import re

TOKEN_TYPES = {
    "WHITESPACE": r"\s+",
    "IDENTIFIER": r"[a-zA-Z_]\w*",
    "SEMICOLON": r";",
    "NUMBER": r"\d+",
    "ASSIGNMENT": r"=",
    "ADDITION": r"\+"
}

class ASTNode:
    pass

class NumberNode(ASTNode):
    def __init__(self, value):
        self.value = int(value)

class IdentifierNode(ASTNode):
    def __init__(self, name):
        self.name = name

class BinaryOpNode(ASTNode):
    def __init__(self, left, op, right):
        self.left = left
        self.op = op
        self.right = right

class AssignmentNode(ASTNode):
    def __init__(self, identifier, value):
        self.identifier = identifier
        self.value = value

class Tokenizer:
    def __init__(self):
        pass
    
    def tokenize(self, source):
        tokens = []
        position = 0
        while position < len(source):
            match = None
            for token_type, pattern in TOKEN_TYPES.items():
                regex = re.compile(pattern)
                match = regex.match(source, position)
                if match:
                    if token_type != "WHITESPACE":
                        tokens.append((token_type, match.group(0)))
                    position = match.end()
                    break
            if not match:
                raise SyntaxError(f"Unexpected character: {source[position]}")
        return tokens

class Parser:
    def __init__(self):
        self.current = 0

    def parse(self, tokens):
        self.tokens = tokens
        statements = []
        while self.current < len(self.tokens):
            statements.append(self.parse_statement())
        return statements

    def parse_statement(self):
        if self.match("IDENTIFIER"):
            return self.parse_assignment()
        else:
            raise SyntaxError(f"Unexpected token: {self.peek()}")

    def parse_assignment(self):
        # Expect IDENTIFIER
        identifier = self.consume("IDENTIFIER")
        # Expect ASSIGNMENT
        self.consume("ASSIGNMENT")
        # Expect an expression (NUMBER or IDENTIFIER or something else)
        value = self.parse_expression()
        # Expect SEMICOLON
        self.consume("SEMICOLON")
        return AssignmentNode(identifier, value)

    def parse_expression(self):
        left = self.parse_term()  # Start by parsing the first term (e.g., a number or identifier)

        # Handle addition (for now, we only have addition)
        while self.match("ADDITION"):
            op = self.consume("ADDITION")
            right = self.parse_term()  # Parse the right side of the addition
            left = BinaryOpNode(left, op, right)

        return left

    def parse_term(self):
        # This handles parsing a term, which could be a number or identifier
        if self.match("NUMBER"):
            return NumberNode(self.consume("NUMBER"))
        elif self.match("IDENTIFIER"):
            return IdentifierNode(self.consume("IDENTIFIER"))
        else:
            raise SyntaxError(f"Unexpected token: {self.peek()}")

    def consume(self, token_type):
        if self.match(token_type):
            token = self.tokens[self.current]
            self.current += 1
            return token[1]  # Return token value (e.g., 'x', '5', etc.)
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
        self.variables = {}

    def interpret(self, ast):
        for node in ast:
            self.execute(node)

    def execute(self, node):
        if isinstance(node, AssignmentNode):
            self.execute_assignment(node)
        elif isinstance(node, BinaryOpNode):
            return self.execute_binary_op(node)
        elif isinstance(node, NumberNode):
            return node.value
        elif isinstance(node, IdentifierNode):
            return self.variables.get(node.name, 0)  # Default to 0 if the variable isn't found
        else:
            raise ValueError(f"Unknown node type: {type(node)}")

    def execute_assignment(self, node):
        value = self.execute(node.value)
        self.variables[node.identifier] = value
        print(f"{node.identifier} = {value}")

    def execute_binary_op(self, node):
        left = self.execute(node.left)
        right = self.execute(node.right)
        if node.op == "+":
            return left + right
        else:
            raise ValueError(f"Unknown operator: {node.op}")

def main():
    file = """Frederick = 69+69;"""

    tokenizer = Tokenizer()
    tokens = tokenizer.tokenize(file)

    parser = Parser()
    ast = parser.parse(tokens)

    interpreter = Interpreter()
    interpreter.interpret(ast)

if __name__ == "__main__":
    main()
