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

TOKEN_TYPES = {
    WHITESPACE: r"\s+",
    IDENTIFIER: r"[a-zA-Z_]\w*",
    SEMICOLON: r";",
    NUMBER: r"\d+",
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
    COMMA: r",",
    TRUE: r"\$",
    FALSE: r"%"
}

class IdentifierPrimitive():
    def __init__(self, name):
        self.name = name

class NumberPrimitive():
    def __init__(self, value):
        self.value = int(value)

class BooleanPrimitive():
    def __init__(self, value):
        self.value = value

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

class FunctionCallPrimitive():
    def __init__(self, name, args):
        self.name = name
        self.args = args

class Tokenizer:
    def __init__(self):
        self.source = ""
    
    def tokenize(self, source):
        self.source = source
        tokens = []
        position = 0
        while position < len(self.source):
            match = None
            for token_type, pattern in TOKEN_TYPES.items():
                regex = re.compile(pattern)
                match = regex.match(self.source, position)
                if match:
                    if token_type != WHITESPACE:
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
        if self.match(IDENTIFIER):
            if self.predict()[0] == ASSIGNMENT:
                statement =  self.parse_assignment()
            elif self.predict()[0] == LEFTPARENTHESES:
                statement = self.parse_function_call()
            else:
                statement = self.parse_or()
        else:
            raise SyntaxError(f"Unexpected token: {self.peek()[1]}")
        
        self.consume(SEMICOLON)
        return statement

    def parse_assignment(self):
        """Parse assignment statement"""
        identifier = self.consume(IDENTIFIER)
        self.consume(ASSIGNMENT)
        value = self.parse_or()
        return AssignmentPrimitive(identifier, value)

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
        elif self.match(IDENTIFIER):
            if self.predict()[0] == LEFTPARENTHESES:
                return self.parse_function_call()
            return IdentifierPrimitive(self.consume(IDENTIFIER))
        elif self.match(LEFTPARENTHESES):
            self.consume(LEFTPARENTHESES)
            expression = self.parse_or()
            self.consume(RIGHTPARENTHESES)
            return expression
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
        # likely will turn this into the monolith later
        self.variables = {}

    def interpret(self, ast):
        for primitive in ast:
            self.execute(primitive)

    def execute(self, primitive):
        if isinstance(primitive, AssignmentPrimitive):
            self.execute_assignment(primitive)
        elif isinstance(primitive, BinaryOperationPrimitive):
            return self.execute_binary_operation(primitive)
        elif isinstance(primitive, UnaryOperationPrimitive):
            return self.execute_unary_operation(primitive)
        elif isinstance(primitive, NumberPrimitive):
            return primitive.value
        elif isinstance(primitive, BooleanPrimitive):
            return primitive.value
        elif isinstance(primitive, IdentifierPrimitive):
            return self.variables[primitive.name]
        elif isinstance(primitive, FunctionCallPrimitive):
            return self.execute_function_call(primitive)
        else:
            raise ValueError(f"Unknown primitive type: {type(primitive)}")

    def execute_print(self, args):
        values = [self.execute(arg) for arg in args]
        print(*values)
        return True
    
    def execute_hello(self):
        print("hello world!")
        return True

    def execute_magic_number(self):
        return random.randint(10000000, 99999999)

    def execute_function_call(self, primitive):
        """Execute builtins and user defined functions"""
        builtins = {
            "print": lambda x: self.execute_print(x),
            "hello": lambda *args: self.execute_hello(),
            "magic_number": lambda *args: self.execute_magic_number()
        }

        result = builtins.get(primitive.name)

        if result:
            return result(primitive.args)
        else:
            raise ValueError(f"Unknown function: {primitive.name}")

    def execute_assignment(self, primitive):
        value = self.execute(primitive.value)
        self.variables[primitive.identifier] = value

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

def main():

    source = """
    print($ == %);
    """

    tokenizer = Tokenizer()
    tokens = tokenizer.tokenize(source)
    # print(tokens)

    parser = Parser()
    ast = parser.parse(tokens)
    # print(ast[0].name)

    interpreter = Interpreter()
    interpreter.interpret(ast)

if __name__ == "__main__":
    main()
