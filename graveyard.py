import re
import random
import os

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
REFERENCE = 49
RANGE = 50
PERIOD = 51
PATH = 52

TOKEN_TYPES = {
    WHITESPACE: r"\s+",
    SINGLELINECOMMENT: r"//.*?$",
    MULTILINECOMMENT: r"/\*.*?\*/",
    PATH: r'<(/[a-zA-Z0-9_/\\]+|\./[a-zA-Z0-9_/\\]+|[a-zA-Z]:[\\a-zA-Z0-9_/\\]+|\.\\[a-zA-Z0-9_/\\]+)>(?:#([a-zA-Z_][a-zA-Z0-9_]*(?:,[a-zA-Z_][a-zA-Z0-9_]*)*))?;',
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
    STRING: r'"(?:\\\"|[^"\\\n])*"',
    FORMATTEDSTRING: r"'(?:\\'|[^'\\\n])*'",
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
    COLON: r":",
    REFERENCE: r"#",
    RANGE: r"\.\.\."
}

class IdentifierPrimitive():
    def __init__(self, name):
        self.name = name

class BooleanPrimitive():
    def __init__(self, value):
        self.value = value

class NumberPrimitive():
    def __init__(self, value):
        if "." in value:
            self.value = float(value)
        else:
            self.value = int(value)

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

class ArrayLookupPrimitive:
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

class HashtableLookupPrimitive:
    def __init__(self, identifier, key):
        self.identifier = identifier
        self.key = key

class HashtableAssignmentPrimitive:
    def __init__(self, identifier, key, value):
        self.identifier = identifier
        self.key = key
        self.value = value

class RangePrimitive:
    def __init__(self, start, end):
        self.start = start
        self.end = end

class ContinuePrimitive:
    pass

class BreakPrimitive:
    pass

class ContinueException(Exception):
    pass

class BreakException(Exception):
    pass

class Graveyard:
    def __init__(self):
        self.source = ""
        self.tokens = []
        self.current = 0
        self.primitives = []
        self.monolith = [{}]

    def tokenize(self, source):
        self.source = source
        tokens = []
        position = 0
        
        while position < len(self.source):
            match = None
            for token_type, pattern in TOKEN_TYPES.items():
                regex_flags = re.DOTALL if token_type == MULTILINECOMMENT else re.MULTILINE  
                regex = re.compile(pattern, regex_flags)
                match = regex.match(self.source, position)
                if match:
                    if token_type in {SINGLELINECOMMENT, MULTILINECOMMENT}:
                        position = match.end()
                        break
                    elif token_type != WHITESPACE:
                        tokens.append((token_type, match.group(0)))
                    position = match.end()
                    break
            if not match:
                raise SyntaxError(f"Unexpected character: {self.source[position]}")
        
        self.tokens = tokens[:]
        self.process_library_references()

    def process_library_references(self):
        offset = 0
        for index, token in enumerate(self.tokens[:]):
            if token[0] == PATH:
                new_tokens = self.evaluate_library_reference(token[1])
                insert_index = index + offset
                self.tokens[insert_index:insert_index + 1] = new_tokens
                offset += len(new_tokens) - 1

    def tokenize_library(self, source):
        tokens = []
        position = 0
        
        while position < len(source):
            match = None
            for token_type, pattern in TOKEN_TYPES.items():
                regex_flags = re.DOTALL if token_type == MULTILINECOMMENT else re.MULTILINE  
                regex = re.compile(pattern, regex_flags)
                match = regex.match(source, position)
                if match:
                    if token_type in {SINGLELINECOMMENT, MULTILINECOMMENT}:
                        position = match.end()
                        break
                    elif token_type != WHITESPACE:
                        tokens.append((token_type, match.group(0)))
                    position = match.end()
                    break
            if not match:
                raise SyntaxError(f"Unexpected character: {source[position]}")

        return tokens

    def evaluate_library_reference(self, path):
        fixed_path = path.replace("\\", "/")
        
        if "#" in fixed_path:
            split = fixed_path.split("#", 1)
            library_path, elements = split[0][1:-1], split[1][:-1]
            element_names = elements.split(",")
        else:
            library_path, element_names = fixed_path[1:-2], None

        base_path = os.path.dirname(library_path)
        library_name = library_path.split("/")[-1]

        expected_file_name = library_name + ".graveyard"
        available_libraries = os.listdir(base_path)

        if expected_file_name not in available_libraries:
            raise ReferenceError(f"Cannot find library {library_name}")

        with open(os.path.join(base_path, expected_file_name), 'r') as file:
            library_source = file.read()

        referenced_tokens = self.tokenize_library(library_source)

        if element_names:
            referenced_tokens = self.extract_specific_elements(referenced_tokens, element_names)

        return referenced_tokens

    def extract_specific_elements(self, tokens, element_names):
        filtered_tokens = []
        capture = False
        brace_depth = 0

        for token in tokens:
            if token[0] == IDENTIFIER and token[1] in element_names:
                capture = True
                brace_depth = 0

            if capture:
                filtered_tokens.append(token)

                if token[0] == LEFTBRACE:  
                    brace_depth += 1
                elif token[0] == RIGHTBRACE:
                    brace_depth -= 1

                if brace_depth == 0 and token[0] == RIGHTBRACE:
                    capture = False

        return filtered_tokens

    def parse(self):
        primitives = []
        while self.current < len(self.tokens):
            primitives.append(self.parse_statement())
        self.primitives = primitives

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
            elif self.predict()[0] == REFERENCE:
                statement = self.parse_hashtable_assignment()
                self.consume(SEMICOLON)
            elif self.predict()[0] == PARAMETER or self.predict()[0] == LEFTBRACE:
                statement = self.parse_function_definition()
            else:
                statement = self.parse_or()
                self.consume(SEMICOLON)
        else:
            raise SyntaxError(f"Unexpected token: {self.peek()[1]}")

        return statement

    def parse_hashtable_assignment(self):
        identifier = self.consume(IDENTIFIER)
        self.consume(REFERENCE)
        key = self.parse_or()
        self.consume(ASSIGNMENT)
        value = self.parse_or()
        return HashtableAssignmentPrimitive(identifier, key, value)

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
    
    def parse_array_lookup(self):
        identifier = self.consume(IDENTIFIER)

        while self.match(LEFTBRACKET):
            self.consume(LEFTBRACKET)
            index = self.parse_or()
            self.consume(RIGHTBRACKET)
            identifier = ArrayLookupPrimitive(identifier, index)
        
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
    
    def parse_hashtable_lookup(self):
        identifier = self.consume(IDENTIFIER)
        self.consume(REFERENCE)
        key = self.parse_or()
        return HashtableLookupPrimitive(identifier, key)

    def parse_numbers_parentheses(self):
        if self.match(NUMBER):
            # return NumberPrimitive(self.consume(NUMBER))
            if self.match(NUMBER):
                left = NumberPrimitive(self.consume(NUMBER))
                if self.match(RANGE):
                    self.consume(RANGE)
                    right = self.parse_numbers_parentheses()
                    return RangePrimitive(left, right)

                return left
        elif self.match(STRING):
            return StringPrimitive(self.consume(STRING))
        elif self.match(FORMATTEDSTRING):
            return FormattedStringPrimitive(self.consume(FORMATTEDSTRING))        
        elif self.match(IDENTIFIER):
            if self.predict()[0] == LEFTBRACKET:
                return self.parse_array_lookup()
            elif self.predict()[0] == REFERENCE:
                return self.parse_hashtable_lookup()
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

    def interpret(self):
        for primitive in self.primitives:
            self.execute(primitive)

    def push_scope(self):
        self.monolith.append({})

    def pop_scope(self):
        if len(self.monolith) > 1:
            self.monolith.pop()
        else:
            raise RuntimeError("Cannot pop global scope")
        
    def get_variable(self, name):
        for scope in reversed(self.monolith):
            if name in scope:
                return scope[name]
        raise NameError(f"Variable '{name}' is not defined")
    
    def set_variable(self, name, value, global_scope=False):
        if global_scope:
            self.monolith[0][name] = value
        else:
            self.monolith[-1][name] = value

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
            StringPrimitive: lambda p: self.execute_string(p),
            FormattedStringPrimitive: lambda p: self.execute_formatted_string(p),
            ArrayPrimitive: lambda p: self.execute_array(p),
            ArrayLookupPrimitive: lambda p: self.execute_array_lookup(p),
            ArrayAssignmentPrimitive: lambda p: self.execute_array_assignment(p),
            ArrayAppendPrimitive: lambda p: self.execute_array_append(p),
            NullPrimitive: lambda p: p.value,
            BooleanPrimitive: lambda p: p.value,
            IdentifierPrimitive: lambda p: self.execute_identifier(p),
            FunctionCallPrimitive: lambda p: self.execute_function_call(p),
            FunctionDefinitionPrimitive: lambda p: self.execute_function_definition(p),
            IfStatementPrimitive: lambda p: self.execute_if_statement(p),
            WhileStatementPrimitive: lambda p: self.execute_while_statement(p),
            ForStatementPrimitive: lambda p: self.execute_for_statement(p),
            ContinuePrimitive: lambda p: self.execute_continue(p),
            BreakPrimitive: lambda p: self.execute_break(p),
            HashtablePrimitive: lambda p: self.execute_hashtable(p),
            HashtableLookupPrimitive: lambda p: self.execute_hashtable_lookup(p),
            HashtableAssignmentPrimitive: lambda p: self.execute_hashtable_assignment(p),
            RangePrimitive: lambda p: self.execute_range(p)
        }

        primitive_type = type(primitive)
        if primitive_type in execute_map:
            return execute_map[primitive_type](primitive)

        raise ValueError(f"Unknown primitive type: {primitive_type}")

    def execute_identifier(self, primitive):
        for scope in reversed(self.monolith):
            if primitive.name in scope:
                return scope[primitive.name]
        raise ValueError(f"Undefined variable: {primitive.name}")

    def execute_function_definition(self, primitive):
        self.monolith[0][primitive.name] = primitive

    def execute_range(self, primitive):
        start = self.execute(primitive.start)
        end = self.execute(primitive.end)

        if type(start) != int:
            start = int(start)
        if type(end) != int:
            end = int(end)

        if start < end:
            return list(range(start, end + 1))
        elif start > end:
            return list(range(start, end - 1, -1))
        elif start == end:
            return [start]

    def execute_string(self, primitive):
        return primitive.value[1:-1].replace('\\"', '"')

    def execute_hashtable(self, primitive):
        key_audit = {}

        for key in primitive.value:
            evaluated_key = self.execute(key)

            if type(evaluated_key) == int or type(evaluated_key) == str:
                key_audit[evaluated_key] = self.execute(primitive.value[key])
            elif type(evaluated_key) == float:
                raise TypeError("Hashtable keys cannot be float, must be integer or string")

        return key_audit
    
    def execute_hashtable_lookup(self, primitive):
        if primitive.identifier not in self.monolith[-1] and primitive.identifier not in self.monolith[0]:
            raise NameError(f"Hashtable '{primitive.identifier}' is not defined")

        hashtable = self.monolith[-1].get(primitive.identifier, self.monolith[0].get(primitive.identifier))

        if type(hashtable) != dict:
            raise TypeError(f"Variable {primitive.identifier} is not a hashtable")

        key = self.execute(primitive.key)

        if key not in hashtable:
            raise KeyError(f"Key {key} not found in hashtable")

        return hashtable[key]
    
    def execute_cast_boolean(self, args):
        value = None
        if type(args[0]) == IdentifierPrimitive:
            value = self.monolith[args[0].name]
        else:
            value = self.execute(args[0])
        return bool(value)

    def execute_cast_integer(self, args):
        value = None
        if type(args[0]) == IdentifierPrimitive:
            value = self.monolith[args[0].name]
        else:
            value = self.execute(args[0])
        return int(value)
    
    def execute_cast_float(self, args):
        value = None
        if type(args[0]) == IdentifierPrimitive:
            value = self.monolith[args[0].name]
        else:
            value = self.execute(args[0])
        return float(value)
    
    def execute_cast_string(self, args):
        value = None
        if type(args[0]) == IdentifierPrimitive:
            value = self.monolith[args[0].name]
        else:
            value = self.execute(args[0])
        return str(value)
    
    def execute_cast_array(self, args):
        value = None
        if type(args[0]) == IdentifierPrimitive:
            value = self.monolith[args[0].name]
        else:
            value = self.execute(args[0])
        return [value]
    
    def execute_cast_hashtable(self, args):
        key = None
        value = None
        if type(args[0]) == IdentifierPrimitive:
            key = self.monolith[args[0].name]

        elif type(args[0]) == ArrayPrimitive:
            if type(args[0].elements[0]) == IdentifierPrimitive:
                key = self.monolith[args[0].elements[0].name]
            else:
                key = self.execute(args[0].elements[0])

            if len(args[0].elements) > 1:
                if type(args[0].elements[1]) == IdentifierPrimitive:
                    value = self.monolith[args[0].elements[1].name]
                elif type(args[0].elements[1]) == ArrayPrimitive:
                    if type(args[0].elements[1]) == IdentifierPrimitive:
                        value = self.monolith[args[0].elements[1].name]
                    else:
                        value = self.execute(args[0].elements[1])
                else:
                    value = self.execute(args[0].elements[1])
        else:
            key = self.execute(args[0])
            
        if type(key) == float:
                key = int(key)

        if len(args) > 1:
            if type(args[1]) == IdentifierPrimitive:
                value = self.monolith[args[1].name]
            elif type(args[1]) == ArrayPrimitive:
                if type(args[1].elements[0]) == IdentifierPrimitive:
                    value = self.monolith[args[1].elements[0].name]
                else:
                    value = self.execute(args[1].elements[0])
            else:
                value = self.execute(args[1])
            
            return {key: value}
        
        if len(args) == 1 and type(args[0]) == ArrayPrimitive and len(args[0].elements) > 1:
            return {key: value}

        return {key: None}

    def execute_print(self, args):
        values = [self.execute(arg) for arg in args]
        print(*values)
        return True
    
    def execute_type(self, args):
        types = []
        for item in args:
            if type(item) == NullPrimitive:
                types.append("null")
            elif type(item) == BooleanPrimitive:
                types.append("boolean")
            elif type(item) == NumberPrimitive:
                if type(item.value) == int:
                    types.append("integer")
                elif type(item.value) == float:
                    types.append("float")
            elif type(item) == StringPrimitive or type(item) == FormattedStringPrimitive:
                if type(item.value) == str:
                    types.append("string")
            elif type(item) == ArrayPrimitive:
                if type(item.elements) == list:
                    types.append("array")
            elif type(item) == HashtablePrimitive:
                if type(item.value) == dict:
                    types.append("hashtable")
            elif type(item) == IdentifierPrimitive:
                if self.monolith[item.name] == None:
                    types.append("null")
                elif type(self.monolith[item.name]) == bool:
                    types.append("boolean")
                elif type(self.monolith[item.name]) == int:
                    types.append("integer")
                elif type(self.monolith[item.name]) == float:
                    types.append("float")
                elif type(self.monolith[item.name]) == str:
                    types.append("string")
                elif type(self.monolith[item.name]) == list:
                    types.append("array")
                elif type(self.monolith[item.name]) == dict:
                    types.append("hashtable")

        if len(types) == 0:
            return None
        elif len(types) == 1:
            return types[0]
        else:
            return types
    
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
            "b": lambda args: self.execute_cast_boolean(args),
            "i": lambda args: self.execute_cast_integer(args),
            "f": lambda args: self.execute_cast_float(args),
            "s": lambda args: self.execute_cast_string(args),
            "a": lambda args: self.execute_cast_array(args),
            "h": lambda args: self.execute_cast_hashtable(args),
            "print": lambda args: self.execute_print(args),
            "type": lambda args: self.execute_type(args),
            "hello": lambda *args: self.execute_hello(),
            "magic_number": lambda *args: self.execute_magic_number(),
            "magic_weight": lambda *args: self.execute_magic_weight(),
            "magic_uid": lambda *args: self.execute_magic_uid()
        }

        if primitive.name in builtins:
            return builtins[primitive.name](primitive.arguments)
        
        elif primitive.name in self.monolith[0]:
            function = self.monolith[0][primitive.name]

            if len(function.parameters) != len(primitive.arguments):
                raise ValueError(f"Incorrect number of arguments for function {primitive.name}")
            
            self.push_scope()

            try:
                for parameter, argument in zip(function.parameters, primitive.arguments):
                    self.monolith[-1][parameter] = self.execute(argument)

                result = None
                for statement in function.body:
                    result = self.execute(statement)

                return self.execute(function.return_value) if function.return_value is not None else result

            finally:
                self.pop_scope()

        else:
            raise ValueError(f"Unknown function: {primitive.name}")

    def execute_assignment(self, primitive):
        value = self.execute(primitive.value)
        self.monolith[-1][primitive.identifier] = value

    def execute_addition_assignment(self, primitive):
        # Search through scopes to find the correct one
        scope_to_update = None
        for scope in reversed(self.monolith):
            if primitive.identifier in scope:
                scope_to_update = scope
                break

        if scope_to_update is None:
            raise NameError(f"Variable '{primitive.identifier}' is not defined")

        current_value = scope_to_update[primitive.identifier]
        new_value = self.execute(primitive.value)

        if isinstance(current_value, str) or isinstance(new_value, str):
            new_value = str(current_value) + str(new_value)
        else:
            new_value = current_value + new_value

        scope_to_update[primitive.identifier] = new_value
        return new_value

    def execute_subtraction_assignment(self, primitive):
        # Search through scopes to find the correct one
        scope_to_update = None
        for scope in reversed(self.monolith):
            if primitive.identifier in scope:
                scope_to_update = scope
                break

        if scope_to_update is None:
            raise NameError(f"Variable '{primitive.identifier}' is not defined")

        current_value = scope_to_update[primitive.identifier]
        new_value = self.execute(primitive.value)

        new_value = current_value - new_value

        # Update the variable in the correct scope
        scope_to_update[primitive.identifier] = new_value
        return new_value

    def execute_multiplication_assignment(self, primitive):
        # Search through scopes to find the correct one
        scope_to_update = None
        for scope in reversed(self.monolith):
            if primitive.identifier in scope:
                scope_to_update = scope
                break

        if scope_to_update is None:
            raise NameError(f"Variable '{primitive.identifier}' is not defined")

        current_value = scope_to_update[primitive.identifier]
        new_value = self.execute(primitive.value)

        new_value = current_value * new_value

        # Update the variable in the correct scope
        scope_to_update[primitive.identifier] = new_value
        return new_value

    def execute_division_assignment(self, primitive):
        # Search through scopes to find the correct one
        scope_to_update = None
        for scope in reversed(self.monolith):
            if primitive.identifier in scope:
                scope_to_update = scope
                break

        if scope_to_update is None:
            raise NameError(f"Variable '{primitive.identifier}' is not defined")

        current_value = scope_to_update[primitive.identifier]
        new_value = self.execute(primitive.value)

        new_value = current_value / new_value

        # Update the variable in the correct scope
        scope_to_update[primitive.identifier] = new_value
        return new_value

    def execute_exponentiation_assignment(self, primitive):
        # Search through scopes to find the correct one
        scope_to_update = None
        for scope in reversed(self.monolith):
            if primitive.identifier in scope:
                scope_to_update = scope
                break

        if scope_to_update is None:
            raise NameError(f"Variable '{primitive.identifier}' is not defined")

        current_value = scope_to_update[primitive.identifier]
        new_value = self.execute(primitive.value)

        new_value = current_value ** new_value

        # Update the variable in the correct scope
        scope_to_update[primitive.identifier] = new_value
        return new_value

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
        if primitive.op == "++" or primitive.op == "--":
            # Search for the variable in the scopes
            scope_to_update = None
            for scope in reversed(self.monolith):
                if primitive.right in scope:
                    scope_to_update = scope
                    break

            if scope_to_update is None:
                raise NameError(f"Variable '{primitive.right}' is not defined")

            # Retrieve the current value and update it
            operand = scope_to_update[primitive.right]
            if primitive.op == "++":
                operand += 1
            elif primitive.op == "--":
                operand -= 1

            # Update the variable in the correct scope
            scope_to_update[primitive.right] = operand
            return operand

        # For the negation operator "!"
        operand = self.execute(primitive.right)

        operations = {
            "!": lambda x: not x,
        }

        operation = operations.get(primitive.op)

        if operation is None:
            raise ValueError(f"Unknown operator: {primitive.op}")

        return operation(operand)

    def execute_formatted_string(self, primitive):
        escaped_string = primitive.value[1:-1].replace("\\'", "'")
        formatted_string = re.sub(r"\{(\w+)\}", lambda match: str(self.monolith[-1].get(match.group(1), self.monolith[0].get(match.group(1), match.group(0)))), escaped_string)
        return formatted_string
    
    def execute_array(self, primitive):
        return [self.execute(element) for element in primitive.elements]
    
    def execute_array_lookup(self, primitive):
        if primitive.identifier not in self.monolith[-1] and primitive.identifier not in self.monolith[0]:
            raise NameError(f"Array '{primitive.identifier}' is not defined")

        array = self.monolith[-1].get(primitive.identifier, self.monolith[0].get(primitive.identifier))

        if type(array) != list:
            raise TypeError(f"Variable {primitive.identifier} is not an array")

        index = self.execute(primitive.index)

        if type(index) != int:
            raise TypeError("Array indices must be integers")

        return array[index]
    
    def execute_array_assignment(self, primitive):
        if type(self.monolith[primitive.identifier]) != list:
            raise TypeError(f"Variable {primitive.identifier} is not an array")
        index = self.execute(primitive.index)
        value = self.execute(primitive.value)
        if type(index) != int:
            raise TypeError("Array indices must be integers")
        self.monolith[primitive.identifier][index] = value

    def execute_hashtable_assignment(self, primitive):
        if primitive.identifier not in self.monolith[-1] and primitive.identifier not in self.monolith[0]:
            raise NameError(f"Hashtable '{primitive.identifier}' is not defined")

        hashtable = self.monolith[-1].get(primitive.identifier, self.monolith[0].get(primitive.identifier))

        if type(hashtable) != dict:
            raise TypeError(f"Variable {primitive.identifier} is not a hashtable")

        key = self.execute(primitive.key)

        if type(key) == int or type(key) == str:
            value = self.execute(primitive.value)
        elif type(key) == float:
            raise TypeError("Hashtable keys cannot be float, must be integer or string")

        hashtable[key] = value

    def execute_array_append(self, primitive):
        if primitive.identifier not in self.monolith[-1] and primitive.identifier not in self.monolith[0]:
            raise NameError(f"Array '{primitive.identifier}' is not defined")

        array = self.monolith[-1].get(primitive.identifier, self.monolith[0].get(primitive.identifier))

        if not isinstance(array, list):
            raise TypeError(f"'{primitive.identifier}' is not an array")

        value = self.execute(primitive.value)
        array.append(value)

    def execute_if_statement(self, primitive):
        for condition, body in primitive.condition_blocks:
            if self.execute(condition):
                self.push_scope()
                try:
                    for statement in body:
                        self.execute(statement)
                finally:
                    self.pop_scope()
                return

        if primitive.else_body:
            self.push_scope()
            try:
                for statement in primitive.else_body:
                    self.execute(statement)
            finally:
                self.pop_scope()

    def execute_while_statement(self, primitive):
        while self.execute(primitive.condition):
            self.push_scope()
            try:
                for statement in primitive.body:
                    self.execute(statement)
            except ContinueException:
                continue
            except BreakException:
                break
            finally:
                self.pop_scope()

    def execute_for_statement(self, primitive):
        iterator_name = primitive.iterator
        limit = self.execute(primitive.limit)

        if type(limit) == dict:
            for key in limit.keys():
                self.push_scope()
                try:
                    self.monolith[-1][iterator_name] = key

                    for statement in primitive.body:
                        self.execute(statement)

                finally:
                    self.pop_scope()

        elif type(limit) == list:
            for item in limit:
                self.push_scope()
                try:
                    self.monolith[-1][iterator_name] = item

                    for statement in primitive.body:
                        self.execute(statement)

                finally:
                    self.pop_scope()

        else:
            if type(limit) != int:
                raise TypeError(f"Cannot iterate through {type(limit)} range")

            for i in range(limit):
                self.push_scope()
                try:
                    self.monolith[-1][iterator_name] = i

                    for statement in primitive.body:
                        self.execute(statement)

                finally:
                    self.pop_scope()

    def execute_continue(self, primitive):
        raise ContinueException()

    def execute_break(self, primitive):
        raise BreakException()

def main():
    T = 900
    P = 901
    E = 902
    M = 903
    D = 904
    print("\n")

    source = r"""
    <./standard>#sanity;
    sanity();
    /*
    test_function {
        counter = 42;
        ? $ {
            other = 2;
            ? $ {
                counter ++;
                print(counter);
            }
        }
    }
    test_function();*/
    """
    graveyard = Graveyard()
    mode = E

    if mode == T:
        graveyard.tokenize(source)
        print(graveyard.tokens)
    elif mode == P:
        graveyard.tokenize(source)
        graveyard.parse()
        # print(graveyard.primitives[0])
        print("parsed successfully")
    elif mode == E:
        graveyard.tokenize(source)
        graveyard.parse()
        graveyard.interpret()
    elif mode == M:
        graveyard.tokenize(source)
        graveyard.parse()
        graveyard.interpret()
        print(graveyard.monolith)
    elif mode == D:
        pass

if __name__ == "__main__":
    main()