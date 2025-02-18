import re

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
    OR: r"\|\|"
}

class IdentifierPrimitive():
    def __init__(self, name):
        self.name = name

class NumberPrimitive():
    def __init__(self, value):
        self.value = int(value)

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
            return self.parse_assignment()
        else:
            raise SyntaxError(f"Unexpected token: {self.peek()[1]}")

    def parse_assignment(self):
        # Expect IDENTIFIER
        identifier = self.consume(IDENTIFIER)
        # Expect ASSIGNMENT
        self.consume(ASSIGNMENT)
        # Expect an expression (NUMBER or IDENTIFIER or something else)
        value = self.parse_or()
        # Expect SEMICOLON
        self.consume(SEMICOLON)
        return AssignmentPrimitive(identifier, value)

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
            return IdentifierPrimitive(self.consume(IDENTIFIER))
        elif self.match(LEFTPARENTHESES):
            self.consume(LEFTPARENTHESES)
            expression = self.parse_or()
            self.consume(RIGHTPARENTHESES)
            return expression
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
        elif isinstance(primitive, IdentifierPrimitive):
            return self.variables[primitive.name]
        else:
            raise ValueError(f"Unknown primitive type: {type(primitive)}")

    def execute_assignment(self, primitive):
        value = self.execute(primitive.value)
        self.variables[primitive.identifier] = value
        print(f"{primitive.identifier} = {value}")

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

    print("")
    print((1 >= 1) and not (1 == 1))

    source = """frederick = (1>=1) && ! (1==1);"""

    tokenizer = Tokenizer()
    tokens = tokenizer.tokenize(source)
    # print(tokens)

    parser = Parser()
    ast = parser.parse(tokens)

    interpreter = Interpreter()
    interpreter.interpret(ast)

if __name__ == "__main__":
    main()
