import re

WHITESPACE = 0
IDENTIFIER = 1
SEMICOLON = 2
NUMBER = 3
ASSIGNMENT = 4
ADDITION = 5
SUBTRACTION = 6
MULTIPLICATION = 7
DIVISION = 8

TOKEN_TYPES = {
    WHITESPACE: r"\s+",
    IDENTIFIER: r"[a-zA-Z_]\w*",
    SEMICOLON: r";",
    NUMBER: r"\d+",
    ASSIGNMENT: r"=",
    ADDITION: r"\+",
    SUBTRACTION: r"\-",
    MULTIPLICATION: r"\*",
    DIVISION: r"\/"
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
            raise SyntaxError(f"Unexpected token: {self.peek()}")

    def parse_assignment(self):
        # Expect IDENTIFIER
        identifier = self.consume(IDENTIFIER)
        # Expect ASSIGNMENT
        self.consume(ASSIGNMENT)
        # Expect an expression (NUMBER or IDENTIFIER or something else)
        value = self.parse_addition_subtraction()
        # Expect SEMICOLON
        self.consume(SEMICOLON)
        return AssignmentPrimitive(identifier, value)

    def parse_addition_subtraction(self):
        """Parse addition and subtraction (lowest precedence)"""
        left = self.parse_multiplication_division()

        while self.match(ADDITION, SUBTRACTION):
            op = self.consume(self.tokens[self.current][0])
            right = self.parse_multiplication_division()
            left = BinaryOperationPrimitive(left, op, right)

        return left

    def parse_multiplication_division(self):
        """Parse multiplication and division"""
        left = self.parse_numbers_parentheses()

        while self.match(MULTIPLICATION, DIVISION):
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
        else:
            raise SyntaxError("Expected number, variable, or parenthases")

    def consume(self, token_type):
        if self.match(token_type):
            token = self.tokens[self.current]
            self.current += 1
            return token[1]
        raise SyntaxError(f"Expected {token_type}, found {self.peek()}")

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
        if primitive.op == "+":
            return left + right
        elif primitive.op == "-":
            return left - right
        elif primitive.op == "*":
            return left * right
        elif primitive.op == "/":
            return left / right
        else:
            raise ValueError(f"Unknown operator: {primitive.op}")

def main():
    source = """frederick = 2*3+1+2/4;"""

    print("")
    print(2*3+1+2/4)

    tokenizer = Tokenizer()
    tokens = tokenizer.tokenize(source)

    parser = Parser()
    ast = parser.parse(tokens)

    interpreter = Interpreter()
    interpreter.interpret(ast)

if __name__ == "__main__":
    main()
