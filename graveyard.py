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
ELSE = 33
WHILE = 34
CONTINUE = 35
BREAK = 36
FOR = 37
FORMATTEDSTRING = 38

TOKEN_TYPES = {
    WHITESPACE: r"\s+",
    SINGLELINECOMMENT: r"//.*?$",
    MULTILINECOMMENT: r"#(.*?)#",
    IDENTIFIER: r"[a-zA-Z_]\w*",
    SEMICOLON: r";",
    RETURN: r"->",
    NUMBER: r"\d+(\.\d+)?",
    EQUALITY: r"==",
    ASSIGNMENT: r"=",
    ADDITION: r"\+",
    SUBTRACTION: r"\-",
    EXPONENTIATION: r"\*\*",
    MULTIPLICATION: r"\*",
    DIVISION: r"\/",
    LEFTPARENTHESES: r"\(",
    RIGHTPARENTHESES: r"\)",
    INEQUALITY: r"!=",
    GREATERTHANEQUAL: r">=",
    LESSTHANEQUAL: r"<=",
    GREATERTHAN: r">",
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
    ELSE: r":",
    WHILE: r"~",
    CONTINUE: r"\^",
    BREAK: r"`",
    FOR: r"@"
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
            elif self.predict()[0] == LEFTPARENTHESES:
                statement = self.parse_function_call()
                self.consume(SEMICOLON)
            elif self.predict()[0] == PARAMETER or self.predict()[0] == LEFTBRACE:
                statement = self.parse_function_definition()
            else:
                statement = self.parse_or()
                self.consume(SEMICOLON)
        else:
            raise SyntaxError(f"Unexpected token: {self.peek()[1]}")
        
        return statement

    def parse_assignment(self):
        """Parse assignment statement"""
        identifier = self.consume(IDENTIFIER)
        self.consume(ASSIGNMENT)
        value = self.parse_or()
        return AssignmentPrimitive(identifier, value)

    def parse_function_definition(self):
        """Parse function definitions"""
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

    def parse_function_call(self):
        """Parse function calls"""
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
        """Parse if statements"""
        self.consume(IF)  # Consume '?'
        condition_blocks = []

        # Parse the first condition
        condition = self.parse_or()
        self.consume(LEFTBRACE)
        body = []
        while not self.match(RIGHTBRACE):
            body.append(self.parse_statement())
        self.consume(RIGHTBRACE)
        condition_blocks.append((condition, body))

        # Parse else-if conditions
        while self.match(COMMA):  # Consume ','
            self.consume(COMMA)
            condition = self.parse_or()
            self.consume(LEFTBRACE)
            body = []
            while not self.match(RIGHTBRACE):
                body.append(self.parse_statement())
            self.consume(RIGHTBRACE)
            condition_blocks.append((condition, body))

        # Parse else block if present
        else_body = None
        if self.match(ELSE):  # Consume ':'
            self.consume(ELSE)
            self.consume(LEFTBRACE)
            else_body = []
            while not self.match(RIGHTBRACE):
                else_body.append(self.parse_statement())
            self.consume(RIGHTBRACE)

        return IfStatementPrimitive(condition_blocks, else_body)

    def parse_while_statement(self):
        """Parse while loops"""
        self.consume(WHILE)  # Consume the '~' token
        condition = self.parse_or()  # Parse the loop condition
        self.consume(LEFTBRACE)  # Consume '{'

        body = []
        while not self.match(RIGHTBRACE):
            body.append(self.parse_statement())

        self.consume(RIGHTBRACE)  # Consume '}'
        return WhileStatementPrimitive(condition, body)
    
    def parse_for_statement(self):
        iterator = self.consume(IDENTIFIER)  # Get loop variable name
        self.consume(FOR)  # Consume `@`
        limit = self.parse_or()  # Parse the loop bound
        self.consume(LEFTBRACE)  # Consume `{`
        
        body = []
        while not self.match(RIGHTBRACE):  # Read loop body until `}`
            body.append(self.parse_statement())

        self.consume(RIGHTBRACE)
        return ForStatementPrimitive(iterator, limit, body)

    def parse_or(self):
        """Parse logical OR"""
        left = self.parse_and()

        while self.match(OR):
            op = self.consume(self.tokens[self.current][0])
            right = self.parse_and()
            left = BinaryOperationPrimitive(left, op, right)

        return left

    def parse_and(self):
        """Parse logical AND"""
        left = self.parse_not()

        while self.match(AND):
            op = self.consume(self.tokens[self.current][0])
            right = self.parse_not()
            left = BinaryOperationPrimitive(left, op, right)

        return left
    
    def parse_not(self):
        """Parse logical NOT"""
        if self.match(NOT):
            op = self.consume(NOT)
            right = self.parse_not()
            return UnaryOperationPrimitive(op, right)
        else:
            return self.parse_comparison()

    def parse_comparison(self):
        """Parse comparison"""
        left = self.parse_addition_subtraction()

        while self.match(EQUALITY, INEQUALITY, GREATERTHANEQUAL, LESSTHANEQUAL, GREATERTHAN, LESSTHAN):
            op = self.consume(self.tokens[self.current][0])
            right = self.parse_addition_subtraction()
            left = BinaryOperationPrimitive(left, op, right)

        return left

    def parse_addition_subtraction(self):
        """Parse addition and subtraction"""
        left = self.parse_multiplication_division()

        while self.match(ADDITION, SUBTRACTION):
            op = self.consume(self.tokens[self.current][0])
            right = self.parse_multiplication_division()
            left = BinaryOperationPrimitive(left, op, right)

        return left

    def parse_multiplication_division(self):
        """Parse multiplication and division"""
        left = self.parse_exponentiation()

        while self.match(MULTIPLICATION, DIVISION):
            op = self.consume(self.tokens[self.current][0])
            right = self.parse_exponentiation()
            left = BinaryOperationPrimitive(left, op, right)

        return left
    
    def parse_exponentiation(self):
        """Parse exponentiation"""
        left = self.parse_numbers_parentheses()

        while self.match(EXPONENTIATION):
            op = self.consume(self.tokens[self.current][0])
            right = self.parse_numbers_parentheses()
            left = BinaryOperationPrimitive(left, op, right)

        return left

    def parse_numbers_parentheses(self):
        """Parse numbers and parentheses (highest precedence)"""
        if self.match(NUMBER):
            return NumberPrimitive(self.consume(NUMBER))
        elif self.match(STRING):
            return StringPrimitive(self.consume(STRING)[1:-1])
        elif self.match(FORMATTEDSTRING):
            return FormattedStringPrimitive(self.consume(FORMATTEDSTRING)[1:-1])        
        elif self.match(IDENTIFIER):
            if self.predict()[0] == LEFTPARENTHESES:
                return self.parse_function_call()
            return IdentifierPrimitive(self.consume(IDENTIFIER))
        elif self.match(LEFTPARENTHESES):
            self.consume(LEFTPARENTHESES)
            expression = self.parse_or()
            self.consume(RIGHTPARENTHESES)
            return expression
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
            BinaryOperationPrimitive: lambda p: self.execute_binary_operation(p),
            UnaryOperationPrimitive: lambda p: self.execute_unary_operation(p),
            NumberPrimitive: lambda p: p.value,
            StringPrimitive: lambda p: p.value,
            FormattedStringPrimitive: lambda p: self.execute_formatted_string(p),
            NullPrimitive: lambda p: p.value,
            BooleanPrimitive: lambda p: p.value,
            IdentifierPrimitive: lambda p: self.monolith[p.name],
            FunctionCallPrimitive: lambda p: self.execute_function_call(p),
            FunctionDefinitionPrimitive: lambda p: self.monolith.update({p.name: p}),
            IfStatementPrimitive: lambda p: self.execute_if_statement(p),
            WhileStatementPrimitive: lambda p: self.execute_while_statement(p),
            ForStatementPrimitive: lambda p: self.execute_for_statement(p),
            ContinuePrimitive: lambda p: self.execute_continue(p),
            BreakPrimitive: lambda p: self.execute_break(p)
        }

        primitive_type = type(primitive)
        if primitive_type in execute_map:
            return execute_map[primitive_type](primitive)

        raise ValueError(f"Unknown primitive type: {primitive_type}")

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
        """Execute built-in and user-defined functions"""
        builtins = {
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
            
            # Bind arguments to parameters
            for parameter, argument in zip(function.parameters, primitive.arguments):
                self.monolith[parameter] = self.execute(argument)
            
            # Execute function body
            for statement in function.body:
                self.execute(statement)
            
            # Return the stored return value if one exists
            return self.execute(function.return_value) if function.return_value is not None else None
        else:
            raise ValueError(f"Unknown function: {primitive.name}")

    def execute_assignment(self, primitive):
        value = self.execute(primitive.value)
        self.monolith[primitive.identifier] = value

    def execute_binary_operation(self, primitive):
        left = self.execute(primitive.left)
        right = self.execute(primitive.right)
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
    
    def execute_if_statement(self, primitive):
        """Execute if statements"""
        for condition, body in primitive.condition_blocks:
            if self.execute(condition):  # Evaluate condition
                for statement in body:
                    self.execute(statement)
                return  # Exit after the first matching condition
        
        # If no conditions matched, execute the else body if it exists
        if primitive.else_body:
            for statement in primitive.else_body:
                self.execute(statement)

    def execute_while_statement(self, primitive):
        """Execute a while loop"""
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
        limit = self.execute(primitive.limit)  # Evaluate loop limit
        if type(limit) == float:
            limit = int(limit)

        for i in range(limit):
            self.monolith[iterator_name] = i  # Assign the loop variable

            try:
                for statement in primitive.body:
                    self.execute(statement)
            except ContinueException:
                continue  # Skip remaining statements and start next iteration
            except BreakException:
                break  # Exit loop completely

    def execute_continue(self, primitive):
        raise ContinueException()

    def execute_break(self, primitive):
        raise BreakException()


def main():
    T = 900
    P = 901
    I = 902
    M = 903
    print("\n")

    source = r"""
    
    greet &input_name &input_age {
        greeting = 'Hi, my name is {input_name} and I am {input_age} years old.';
        print(greeting);
    }

    name = "Steve";
    age = 69;

    greet(name, age);
    greet("Fred", 101);

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

if __name__ == "__main__":
    main()
