import re
import random

#@! reoder these to match TOKEN_TYPES once we add them all
WHITESPACE = 0
IDENTIFIER = 1
SEMICOLON = 2
NUMBER = 3
ASSIGNMENT = 4
ADDITION = 5
SUBTRACTION = 6
MULTIPLICATION = 7
DIVISION = 8
LEFTPARENTHESES = 9
RIGHTPARENTHESES = 10
EXPONENTIATION = 11
EQUALITY = 12
INEQUALITY = 13
GREATERTHAN = 14
LESSTHAN = 15
GREATERTHANEQUAL = 16
LESSTHANEQUAL = 17
NOT = 18
AND = 19
OR = 20
COMMA = 21
TRUE = 22
FALSE = 23
NULL = 24
SINGLELINECOMMENT = 25
MULTILINECOMMENT = 26
STRING = 27
LEFTBRACE = 28
RIGHTBRACE = 29
PARAMETER = 30
RETURN = 31
IF = 32
COLON = 33
WHILE = 34
CONTINUE = 35
BREAK = 36
FOR = 37
FORMATTEDSTRING = 38
LEFTBRACKET = 39
RIGHTBRACKET = 40
APPEND = 41
ADDITIONASSIGNMENT = 42
SUBTRACTIONASSIGNMENT = 43
MULTIPLICATIONASSIGNMENT = 44
DIVISIONASSIGNMENT = 45
EXPONENTIATIONASSIGNMENT = 46
INCREMENT = 47
DECREMENT = 48

TOKEN_TYPES = {
    WHITESPACE: r"\s+",
    SINGLELINECOMMENT: r"//.*?$",
    MULTILINECOMMENT: r"/\*.*?\*/",
    IDENTIFIER: r"[a-zA-Z_]\w*",
    SEMICOLON: r";",
    RETURN: r"->",
    NUMBER: r"\d+(\.\d+)?",
    EQUALITY: r"==",
    ASSIGNMENT: r"=",
    INCREMENT: r"\+\+",
    ADDITIONASSIGNMENT: r"\+=",
    ADDITION: r"\+",
    SUBTRACTIONASSIGNMENT: r"-=",
    DECREMENT: r"--",
    SUBTRACTION: r"\-",
    EXPONENTIATIONASSIGNMENT: r"\*\*=",
    EXPONENTIATION: r"\*\*",
    MULTIPLICATIONASSIGNMENT: r"\*=",
    MULTIPLICATION: r"\*",
    DIVISIONASSIGNMENT: r"/=",
    DIVISION: r"\/",
    LEFTPARENTHESES: r"\(",
    RIGHTPARENTHESES: r"\)",
    INEQUALITY: r"!=",
    GREATERTHANEQUAL: r">=",
    LESSTHANEQUAL: r"<=",
    GREATERTHAN: r">",
    APPEND: r"<-",
    LESSTHAN: r"<",
    NOT: r"!",
    AND: r"&&",
    OR: r"\|\|",
    NULL: r"\|",
    COMMA: r",",
    TRUE: r"\$",
    FALSE: r"%",
    STRING: r'"[^"\n]*"',
    FORMATTEDSTRING: r"'([^'\\]*(\\.[^'\\]*)*)'",
    LEFTBRACE: r"\{",
    RIGHTBRACE: r"\}",
    PARAMETER: r"&",
    IF: r"\?",
    WHILE: r"~",
    CONTINUE: r"\^",
    BREAK: r"`",
    FOR: r"@",
    LEFTBRACKET: r"\[",
    RIGHTBRACKET: r"\]",
    COLON: r":"
}

class IdentifierPrimitive():
    def __init__(self, name):
        self.name = name

class BooleanPrimitive():
    def __init__(self, value):
        self.value = value

class NumberPrimitive():
    def __init__(self, value):
        self.value = float(value)

class StringPrimitive:
    def __init__(self, value):
        self.value = value

class NullPrimitive():
    def __init__(self):
        self.value = None

class BinaryOperationPrimitive():
    def __init__(self, left, op, right):
        self.left = left
        self.op = op
        self.right = right

class UnaryOperationPrimitive():
    def __init__(self, op, right):
        self.op = op
        self.right = right

class AssignmentPrimitive():
    def __init__(self, identifier, value):
        self.identifier = identifier
        self.value = value

class FunctionDefinitionPrimitive:
    def __init__(self, name, parameters, body, return_value=None):
        self.name = name
        self.parameters = parameters
        self.body = body
        self.return_value = return_value

class FunctionCallPrimitive():
    def __init__(self, name, arguments):
        self.name = name
        self.arguments = arguments

class IfStatementPrimitive:
    def __init__(self, condition_blocks, else_body):
        self.condition_blocks = condition_blocks
        self.else_body = else_body

class WhileStatementPrimitive:
    def __init__(self, condition, body):
        self.condition = condition
        self.body = body

class ForStatementPrimitive:
    def __init__(self, iterator, limit, body):
        self.iterator = iterator
        self.limit = limit
        self.body = body

class FormattedStringPrimitive:
    def __init__(self, value):
        self.value = value

class ArrayPrimitive:
    def __init__(self, elements):
        self.elements = elements

class ArrayAccessPrimitive:
    def __init__(self, identifier, index):
        self.identifier = identifier
        self.index = index

class ArrayAssignmentPrimitive:
    def __init__(self, identifier, index, value):
        self.identifier = identifier
        self.index = index
        self.value = value
        
class ArrayAppendPrimitive:
    def __init__(self, identifier, value):
        self.identifier = identifier
        self.value = value

class AdditionAssignmentPrimitive:
    def __init__(self, identifier, value):
        self.identifier = identifier
        self.value = value

class SubtractionAssignmentPrimitive:
    def __init__(self, identifier, value):
        self.identifier = identifier
        self.value = value

class MultiplicationAssignmentPrimitive:
    def __init__(self, identifier, value):
        self.identifier = identifier
        self.value = value

class DivisionAssignmentPrimitive:
    def __init__(self, identifier, value):
        self.identifier = identifier
        self.value = value

class ExponentiationAssignmentPrimitive:
    def __init__(self, identifier, value):
        self.identifier = identifier
        self.value = value

class HashtablePrimitive:
    def __init__(self, value):
        self.value = value

class ContinuePrimitive:
    pass

class BreakPrimitive:
    pass

class ContinueException(Exception):
    pass

class BreakException(Exception):
    pass

class Tokenizer:
    def __init__(self):
        self.source = ""
    
    def tokenize(self, source):
        self.source = source
        tokens = []
        position = 0
        skip_until = None
        while position < len(self.source):
            match = None
            for token_type, pattern in TOKEN_TYPES.items():
                regex_flags = re.DOTALL if token_type == MULTILINECOMMENT else re.MULTILINE  
                regex = re.compile(pattern, regex_flags)
                match = regex.match(self.source, position)
                if match:
                    if token_type == SINGLELINECOMMENT:
                        position = match.end()
                        break
                    elif token_type == MULTILINECOMMENT:
                        position = match.end()
                        break
                    elif token_type != WHITESPACE:
                        tokens.append((token_type, match.group(0)))
                    position = match.end()
                    break
            if not match:
                raise SyntaxError(f"Unexpected character: {self.source[position]}")
        return tokens

class Parser:
    def __init__(self):
        self.tokens = []
        self.current = 0

    def parse(self, tokens):
        self.tokens = tokens
        statements = []
        while self.current < len(self.tokens):
            statements.append(self.parse_statement())
        return statements

    def parse_statement(self):
        if self.match(WHILE):
            statement = self.parse_while_statement()
        elif self.match(IDENTIFIER) and self.predict()[0] == FOR:
            statement = self.parse_for_statement()
        elif self.match(CONTINUE):
            self.consume(CONTINUE)
            statement = ContinuePrimitive()
            self.consume(SEMICOLON)
        elif self.match(BREAK):
            self.consume(BREAK)
            statement = BreakPrimitive()
            self.consume(SEMICOLON)
        elif self.match(IF):
            statement = self.parse_if_statement()
        elif self.match(IDENTIFIER):
            if self.predict()[0] == ASSIGNMENT:
                statement = self.parse_assignment()
                self.consume(SEMICOLON)
            elif self.predict()[0] == INCREMENT:
                statement = self.parse_increment()
                self.consume(SEMICOLON)
            elif self.predict()[0] == DECREMENT:
                statement = self.parse_decrement()
                self.consume(SEMICOLON)
            elif self.predict()[0] == ADDITIONASSIGNMENT:
                statement = self.parse_addition_assignment()
                self.consume(SEMICOLON)
            elif self.predict()[0] == SUBTRACTIONASSIGNMENT:
                statement = self.parse_subtraction_assignment()
                self.consume(SEMICOLON)
            elif self.predict()[0] == MULTIPLICATIONASSIGNMENT:
                statement = self.parse_multiplication_assignment()
                self.consume(SEMICOLON)
            elif self.predict()[0] == DIVISIONASSIGNMENT:
                statement = self.parse_division_assignment()
                self.consume(SEMICOLON)
            elif self.predict()[0] == EXPONENTIATIONASSIGNMENT:
                statement = self.parse_exponentiation_assignment()
                self.consume(SEMICOLON)
            elif self.predict()[0] == LEFTPARENTHESES:
                statement = self.parse_function_call()
                self.consume(SEMICOLON)
            elif self.predict()[0] == LEFTBRACKET:
                statement = self.parse_array_assignment()
                self.consume(SEMICOLON)
            elif self.predict()[0] == APPEND:
                statement = self.parse_array_append()
                self.consume(SEMICOLON)
            elif self.predict()[0] == PARAMETER or self.predict()[0] == LEFTBRACE:
                statement = self.parse_function_definition()
            else:
                statement = self.parse_or()
                self.consume(SEMICOLON)
        else:
            raise SyntaxError(f"Unexpected token: {self.peek()[1]}")
        
        return statement

    def parse_array_assignment(self):
        identifier = self.consume(IDENTIFIER)
        index = None
        if self.match(LEFTBRACKET):
            self.consume(LEFTBRACKET)
            index = self.parse_or()
            self.consume(RIGHTBRACKET)
        self.consume(ASSIGNMENT)
        value = self.parse_or()
        return ArrayAssignmentPrimitive(identifier, index, value)

    def parse_assignment(self):
        identifier = self.consume(IDENTIFIER)
        self.consume(ASSIGNMENT)
        value = self.parse_or()
        return AssignmentPrimitive(identifier, value)

    def parse_function_definition(self):
        name = self.peek()[1]
        self.consume(IDENTIFIER)
        
        parameters = []
        while self.match(PARAMETER):
            self.consume(PARAMETER)
            parameters.append(self.peek()[1])
            self.consume(IDENTIFIER)
        
        self.consume(LEFTBRACE)
        body = []
        return_value = None
        
        while not self.match(RIGHTBRACE):
            if self.match(RETURN):
                self.consume(RETURN)
                return_value = self.parse_or()
                self.consume(SEMICOLON)
            else:
                body.append(self.parse_statement())
        
        self.consume(RIGHTBRACE)
        return FunctionDefinitionPrimitive(name, parameters, body, return_value)

    def parse_addition_assignment(self):
        identifier = self.consume(IDENTIFIER)
        self.consume(ADDITIONASSIGNMENT)
        value = self.parse_or()
        return AdditionAssignmentPrimitive(identifier, value)
    
    def parse_subtraction_assignment(self):
        identifier = self.consume(IDENTIFIER)
        self.consume(SUBTRACTIONASSIGNMENT)
        value = self.parse_or()
        return SubtractionAssignmentPrimitive(identifier, value)
    
    def parse_multiplication_assignment(self):
        identifier = self.consume(IDENTIFIER)
        self.consume(MULTIPLICATIONASSIGNMENT)
        value = self.parse_or()
        return MultiplicationAssignmentPrimitive(identifier, value)
    
    def parse_division_assignment(self):
        identifier = self.consume(IDENTIFIER)
        self.consume(DIVISIONASSIGNMENT)
        value = self.parse_or()
        return DivisionAssignmentPrimitive(identifier, value)
    
    def parse_exponentiation_assignment(self):
        identifier = self.consume(IDENTIFIER)
        self.consume(EXPONENTIATIONASSIGNMENT)
        value = self.parse_or()
        return ExponentiationAssignmentPrimitive(identifier, value)

    def parse_function_call(self):
        name = self.peek()[1]
        self.consume(IDENTIFIER)
        self.consume(LEFTPARENTHESES)
        args = []

        if not self.match(RIGHTPARENTHESES):
            while True:
                args.append(self.parse_or())
                if self.match(COMMA):
                    self.consume(COMMA)
                else:
                    break

        self.consume(RIGHTPARENTHESES)

        return FunctionCallPrimitive(name, args)

    def parse_if_statement(self):
        self.consume(IF)
        condition_blocks = []
        condition = self.parse_or()
        self.consume(LEFTBRACE)
        body = []
        while not self.match(RIGHTBRACE):
            body.append(self.parse_statement())
        self.consume(RIGHTBRACE)
        condition_blocks.append((condition, body))

        while self.match(COMMA):
            self.consume(COMMA)
            condition = self.parse_or()
            self.consume(LEFTBRACE)
            body = []
            while not self.match(RIGHTBRACE):
                body.append(self.parse_statement())
            self.consume(RIGHTBRACE)
            condition_blocks.append((condition, body))

        else_body = None
        if self.match(COLON):
            self.consume(COLON)
            self.consume(LEFTBRACE)
            else_body = []
            while not self.match(RIGHTBRACE):
                else_body.append(self.parse_statement())
            self.consume(RIGHTBRACE)

        return IfStatementPrimitive(condition_blocks, else_body)

    def parse_while_statement(self):
        self.consume(WHILE)
        condition = self.parse_or()
        self.consume(LEFTBRACE)

        body = []
        while not self.match(RIGHTBRACE):
            body.append(self.parse_statement())

        self.consume(RIGHTBRACE)
        return WhileStatementPrimitive(condition, body)
    
    def parse_for_statement(self):
        iterator = self.consume(IDENTIFIER)
        self.consume(FOR)
        limit = self.parse_or()
        self.consume(LEFTBRACE)
        
        body = []
        while not self.match(RIGHTBRACE):
            body.append(self.parse_statement())

        self.consume(RIGHTBRACE)
        return ForStatementPrimitive(iterator, limit, body)

    def parse_or(self):
        left = self.parse_and()

        while self.match(OR):
            op = self.consume(self.tokens[self.current][0])
            right = self.parse_and()
            left = BinaryOperationPrimitive(left, op, right)

        return left

    def parse_and(self):
        left = self.parse_not()

        while self.match(AND):
            op = self.consume(self.tokens[self.current][0])
            right = self.parse_not()
            left = BinaryOperationPrimitive(left, op, right)

        return left
    
    def parse_not(self):
        if self.match(NOT):
            op = self.consume(NOT)
            right = self.parse_not()
            return UnaryOperationPrimitive(op, right)
        else:
            return self.parse_comparison()

    def parse_comparison(self):
        left = self.parse_addition_subtraction()

        while self.match(EQUALITY, INEQUALITY, GREATERTHANEQUAL, LESSTHANEQUAL, GREATERTHAN, LESSTHAN):
            op = self.consume(self.tokens[self.current][0])
            right = self.parse_addition_subtraction()
            left = BinaryOperationPrimitive(left, op, right)

        return left

    def parse_addition_subtraction(self):
        left = self.parse_multiplication_division()

        while self.match(ADDITION, SUBTRACTION):
            op = self.consume(self.tokens[self.current][0])
            right = self.parse_multiplication_division()
            left = BinaryOperationPrimitive(left, op, right)

        return left

    def parse_multiplication_division(self):
        left = self.parse_exponentiation()

        while self.match(MULTIPLICATION, DIVISION):
            op = self.consume(self.tokens[self.current][0])
            right = self.parse_exponentiation()
            left = BinaryOperationPrimitive(left, op, right)

        return left
    
    def parse_exponentiation(self):
        left = self.parse_numbers_parentheses()

        while self.match(EXPONENTIATION):
            op = self.consume(self.tokens[self.current][0])
            right = self.parse_numbers_parentheses()
            left = BinaryOperationPrimitive(left, op, right)

        return left

    def parse_increment(self):
        identifier = self.consume(IDENTIFIER)
        operator = self.consume(INCREMENT)
        return UnaryOperationPrimitive(operator, identifier)
    
    def parse_decrement(self):
        identifier = self.consume(IDENTIFIER)
        operator = self.consume(DECREMENT)
        return UnaryOperationPrimitive(operator, identifier)

    def parse_array(self):
        self.consume(LEFTBRACKET)
        elements = []

        if not self.match(RIGHTBRACKET):
            while True:
                elements.append(self.parse_or())
                if self.match(COMMA):
                    self.consume(COMMA)
                else:
                    break

        self.consume(RIGHTBRACKET)
        return ArrayPrimitive(elements)
    
    def parse_array_access(self):
        identifier = self.consume(IDENTIFIER)

        while self.match(LEFTBRACKET):
            self.consume(LEFTBRACKET)
            index = self.parse_or()
            self.consume(RIGHTBRACKET)
            identifier = ArrayAccessPrimitive(identifier, index)
        
        return identifier
    
    def parse_array_append(self):
        identifier = self.consume(IDENTIFIER)
        self.consume(APPEND)
        value = self.parse_or()
        return ArrayAppendPrimitive(identifier, value)

    def parse_hashtable(self):
        self.consume(LEFTBRACE)
        hashtable = {}

        while not self.match(RIGHTBRACE):
            key = self.parse_or()
            self.consume(COLON)
            value = self.parse_or()
            hashtable[key] = value

            if not self.match(RIGHTBRACE):
                self.consume(COMMA)

        self.consume(RIGHTBRACE)
        return HashtablePrimitive(hashtable)

    def parse_numbers_parentheses(self):
        if self.match(NUMBER):
            return NumberPrimitive(self.consume(NUMBER))
        elif self.match(STRING):
            return StringPrimitive(self.consume(STRING)[1:-1])
        elif self.match(FORMATTEDSTRING):
            return FormattedStringPrimitive(self.consume(FORMATTEDSTRING)[1:-1])        
        elif self.match(IDENTIFIER):
            if self.predict()[0] == LEFTBRACKET:
                return self.parse_array_access()
            elif self.predict()[0] == LEFTPARENTHESES:
                return self.parse_function_call()
            return IdentifierPrimitive(self.consume(IDENTIFIER))
        elif self.match(LEFTPARENTHESES):
            self.consume(LEFTPARENTHESES)
            expression = self.parse_or()
            self.consume(RIGHTPARENTHESES)
            return expression
        elif self.match(LEFTBRACKET):
            return self.parse_array()
        elif self.match(LEFTBRACE):
            return self.parse_hashtable()
        elif self.match(NULL):
            self.consume(NULL)
            return NullPrimitive()
        elif self.match(TRUE):
            self.consume(TRUE)
            return BooleanPrimitive(True)
        elif self.match(FALSE):
            self.consume(FALSE)
            return BooleanPrimitive(False)
        else:
            raise SyntaxError(f"Expected number, variable, or parenthases got {self.peek()[1]}")

    def consume(self, token_type):
        if self.match(token_type):
            token = self.tokens[self.current]
            self.current += 1
            return token[1]
        raise SyntaxError(f"Expected {token_type}, found {self.peek()[1]}")

    def match(self, *token_types):
        if self.current < len(self.tokens):
            token_type = self.tokens[self.current][0]
            return token_type in token_types
        return False 

    def peek(self):
        if self.current < len(self.tokens):
            return self.tokens[self.current]
        return None
    
    def predict(self, offset=1):
        prediction_index = self.current + offset
        if prediction_index < len(self.tokens):
            return self.tokens[prediction_index]
        return None

class Interpreter:
    def __init__(self):
        self.monolith = {}

    def interpret(self, ast):
        for primitive in ast:
            self.execute(primitive)

    def execute(self, primitive):
        execute_map = {
            AssignmentPrimitive: lambda p: self.execute_assignment(p),
            AdditionAssignmentPrimitive: lambda p: self.execute_addition_assignment(p),
            SubtractionAssignmentPrimitive: lambda p: self.execute_subtraction_assignment(p),
            MultiplicationAssignmentPrimitive: lambda p: self.execute_multiplication_assignment(p),
            DivisionAssignmentPrimitive: lambda p: self.execute_division_assignment(p),
            ExponentiationAssignmentPrimitive: lambda p: self.execute_exponentiation_assignment(p),
            BinaryOperationPrimitive: lambda p: self.execute_binary_operation(p),
            UnaryOperationPrimitive: lambda p: self.execute_unary_operation(p),
            NumberPrimitive: lambda p: p.value,
            StringPrimitive: lambda p: p.value,
            FormattedStringPrimitive: lambda p: self.execute_formatted_string(p),
            ArrayPrimitive: lambda p: self.execute_array(p),
            ArrayAccessPrimitive: lambda p: self.execute_array_access(p),
            ArrayAssignmentPrimitive: lambda p: self.execute_array_assignment(p),
            ArrayAppendPrimitive: lambda p: self.execute_array_append(p),
            NullPrimitive: lambda p: p.value,
            BooleanPrimitive: lambda p: p.value,
            IdentifierPrimitive: lambda p: self.monolith[p.name],
            FunctionCallPrimitive: lambda p: self.execute_function_call(p),
            FunctionDefinitionPrimitive: lambda p: self.monolith.update({p.name: p}),
            IfStatementPrimitive: lambda p: self.execute_if_statement(p),
            WhileStatementPrimitive: lambda p: self.execute_while_statement(p),
            ForStatementPrimitive: lambda p: self.execute_for_statement(p),
            ContinuePrimitive: lambda p: self.execute_continue(p),
            BreakPrimitive: lambda p: self.execute_break(p),
            HashtablePrimitive: lambda p: self.execute_hashtable(p),
        }

        primitive_type = type(primitive)
        if primitive_type in execute_map:
            return execute_map[primitive_type](primitive)

        raise ValueError(f"Unknown primitive type: {primitive_type}")

    def execute_hashtable(self, primitive):
        key_audit = {}

        for key in primitive.value:
            evaluated_key = self.execute(key)

            if type(evaluated_key) == int or type(evaluated_key) == str:
                key_audit[evaluated_key] = self.execute(primitive.value[key])
            elif type(evaluated_key) == float:
                key_audit[int(evaluated_key)] = self.execute(primitive.value[key])

        return key_audit

    def execute_cast_integer(self, args):
        return int(self.monolith[args[0].name])
    
    def execute_cast_float(self, args):
        return float(self.monolith[args[0].name])
    
    def execute_cast_string(self, args):
        return str(self.monolith[args[0].name])
    
    def execute_cast_array(self, args):
        return [self.monolith[args[0].name]]

    def execute_print(self, args):
        values = [self.execute(arg) for arg in args]
        print(*values)
        return True
    
    def execute_hello(self):
        print("hello world!")
        return True

    def execute_magic_number(self):
        return random.randint(10000000, 99999999)

    def execute_magic_weight(self):
        return round(random.random(), 8)
    
    def execute_magic_uid(self):
        return str(hex(random.randint(286331153, 4294967295)))[2:]

    def execute_function_call(self, primitive):
        builtins = {
            "i": lambda args: self.execute_cast_integer(args),
            "f": lambda args: self.execute_cast_float(args),
            "s": lambda args: self.execute_cast_string(args),
            "a": lambda args: self.execute_cast_array(args),
            "print": lambda args: self.execute_print(args),
            "hello": lambda *args: self.execute_hello(),
            "magic_number": lambda *args: self.execute_magic_number(),
            "magic_weight": lambda *args: self.execute_magic_weight(),
            "magic_uid": lambda *args: self.execute_magic_uid()
        }

        if primitive.name in builtins:
            return builtins[primitive.name](primitive.arguments)
        elif primitive.name in self.monolith:
            function = self.monolith[primitive.name]
            if len(function.parameters) != len(primitive.arguments):
                raise ValueError(f"Incorrect number of arguments for function {primitive.name}")
            
            for parameter, argument in zip(function.parameters, primitive.arguments):
                self.monolith[parameter] = self.execute(argument)

            for statement in function.body:
                self.execute(statement)

            return self.execute(function.return_value) if function.return_value is not None else None
        else:
            raise ValueError(f"Unknown function: {primitive.name}")

    def execute_assignment(self, primitive):
        value = self.execute(primitive.value)
        self.monolith[primitive.identifier] = value

    def execute_addition_assignment(self, primitive):
        if primitive.identifier not in self.monolith:
            raise NameError(f"Variable '{primitive.identifier}' is not defined")

        current_value = self.monolith[primitive.identifier]
        new_value = self.execute(primitive.value)
        if type(current_value) == str or type(new_value) == str:
            self.monolith[primitive.identifier] = str(current_value) + str(new_value)
        else:
            self.monolith[primitive.identifier] = current_value + new_value

    def execute_subtraction_assignment(self, primitive):
        if primitive.identifier not in self.monolith:
            raise NameError(f"Variable '{primitive.identifier}' is not defined")

        current_value = self.monolith[primitive.identifier]
        new_value = self.execute(primitive.value)

        self.monolith[primitive.identifier] = current_value - new_value

    def execute_multiplication_assignment(self, primitive):
        if primitive.identifier not in self.monolith:
            raise NameError(f"Variable '{primitive.identifier}' is not defined")

        current_value = self.monolith[primitive.identifier]
        new_value = self.execute(primitive.value)

        self.monolith[primitive.identifier] = current_value * new_value

    def execute_division_assignment(self, primitive):
        if primitive.identifier not in self.monolith:
            raise NameError(f"Variable '{primitive.identifier}' is not defined")

        current_value = self.monolith[primitive.identifier]
        new_value = self.execute(primitive.value)

        self.monolith[primitive.identifier] = current_value / new_value

    def execute_exponentiation_assignment(self, primitive):
        if primitive.identifier not in self.monolith:
            raise NameError(f"Variable '{primitive.identifier}' is not defined")

        current_value = self.monolith[primitive.identifier]
        new_value = self.execute(primitive.value)

        self.monolith[primitive.identifier] = current_value ** new_value

    def execute_binary_operation(self, primitive):
        left = self.execute(primitive.left)
        right = self.execute(primitive.right)

        if type(left) == str or type(right) == str:
            operations = {
                "+": lambda x, y: str(x) + str(y)
            }
        else:
            operations = {
                "+": lambda x, y: x + y,
                "-": lambda x, y: x - y,
                "*": lambda x, y: x * y,
                "/": lambda x, y: x / y,
                "**": lambda x, y: x ** y,
                "==": lambda x, y: x == y,
                "!=": lambda x, y: x != y,
                ">=": lambda x, y: x >= y,
                "<=": lambda x, y: x <= y,
                ">": lambda x, y: x > y,
                "<": lambda x, y: x < y,
                "&&": lambda x, y: x and y,
                "||": lambda x, y: x or y
            }
        operation = operations.get(primitive.op)

        if operation is None:
            raise ValueError(f"Unknown operator: {primitive.op}")

        return operation(left, right)
    
    def execute_unary_operation(self, primitive):
        if primitive.op == "++":
            self.monolith[primitive.right] += 1
            return self.monolith[primitive.right]
        elif primitive.op == "--":
            self.monolith[primitive.right] -= 1
            return self.monolith[primitive.right]

        #@! might want to clean this up and make it consistent later on if there are no other unary operators (including the use of "right" instead of "value")
        right = 0
        if type(primitive.right) != str:
            right = self.execute(primitive.right)

        operations = {
            "!": lambda x: not x,

        }

        operation = operations.get(primitive.op)

        if operation is None:
            raise ValueError(f"Unknown operator: {primitive.op}")

        return operation(right)
    
    def execute_formatted_string(self, primitive):
        formatted_string = re.sub(r"\{(\w+)\}", lambda match: str(self.monolith.get(match.group(1), match.group(0))), primitive.value)
        return formatted_string
    
    def execute_array(self, primitive):
        return [self.execute(element) for element in primitive.elements]
    
    def execute_array_access(self, primitive):
        array = self.monolith[primitive.identifier]
        index = int(self.execute(primitive.index))

        return array[index]
    
    def execute_array_assignment(self, primitive):
        index = int(self.execute(primitive.index))
        value = self.execute(primitive.value)

        self.monolith[primitive.identifier][index] = value

    def execute_array_append(self, primitive):
        if primitive.identifier not in self.monolith:
            raise NameError(f"Array '{primitive.identifier}' is not defined")

        array = self.monolith[primitive.identifier]

        if not isinstance(array, list):
            raise TypeError(f"'{primitive.identifier}' is not an array")

        value = self.execute(primitive.value)
        array.append(value)
    
    def execute_if_statement(self, primitive):
        for condition, body in primitive.condition_blocks:
            if self.execute(condition):
                for statement in body:
                    self.execute(statement)
                return

        if primitive.else_body:
            for statement in primitive.else_body:
                self.execute(statement)

    def execute_while_statement(self, primitive):
        while self.execute(primitive.condition):
            try:
                for statement in primitive.body:
                    self.execute(statement)
            except ContinueException:
                continue
            except BreakException:
                break

    def execute_for_statement(self, primitive):
        iterator_name = primitive.iterator
        limit = self.execute(primitive.limit)
        if type(limit) == float:
            limit = int(limit)

        for i in range(limit):
            self.monolith[iterator_name] = i

            try:
                for statement in primitive.body:
                    self.execute(statement)
            except ContinueException:
                continue
            except BreakException:
                break

    def execute_continue(self, primitive):
        raise ContinueException()

    def execute_break(self, primitive):
        raise BreakException()

def main():
    T = 900
    P = 901
    I = 902
    M = 903
    D = 904
    print("\n")

    source = r"""
    y = 42;
    x = {
        "test_key": magic_number(),
        "another_test": 42,
        69: magic_weight(),
        magic_uid(): "stuff"
    };

    print(x);
    """
    mode = I








    if mode == T:
        tokenizer = Tokenizer()
        tokens = tokenizer.tokenize(source)
        print(tokens)
    elif mode == P:
        tokenizer = Tokenizer()
        tokens = tokenizer.tokenize(source)
        parser = Parser()
        ast = parser.parse(tokens)
        # print(ast[1].op)
        print("parsed successfully")
    elif mode == I:
        tokenizer = Tokenizer()
        tokens = tokenizer.tokenize(source)
        parser = Parser()
        ast = parser.parse(tokens)
        interpreter = Interpreter()
        interpreter.interpret(ast)
    elif mode == M:
        tokenizer = Tokenizer()
        tokens = tokenizer.tokenize(source)
        parser = Parser()
        ast = parser.parse(tokens)
        interpreter = Interpreter()
        interpreter.interpret(ast)
        print(interpreter.monolith)
    elif mode == D:
        pass

if __name__ == "__main__":
    main()
