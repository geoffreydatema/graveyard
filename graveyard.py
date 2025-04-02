import re
import random
import os
import time
from datetime import datetime

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
NAMESPACE = 53
OPENGLOBAL = 54
CLOSEGLOBAL = 55

TOKEN_TYPES = {
    WHITESPACE: r"\s+",
    SINGLELINECOMMENT: r"//.*?$",
    MULTILINECOMMENT: r"/\*.*?\*/",
    PATH: r'@(/[a-zA-Z0-9_/\\]+|\./[a-zA-Z0-9_/\\]+|[a-zA-Z]:[\\a-zA-Z0-9_/\\]+|\.\\[a-zA-Z0-9_/\\]+)(?:#\s*([a-zA-Z_][a-zA-Z0-9_]*\s*(?:,\s*[a-zA-Z_][a-zA-Z0-9_]*\s*)*)\s*)?;',
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
    FORMATTEDSTRING: r"'",
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
    NAMESPACE: r"::",
    COLON: r":",
    REFERENCE: r"#",
    RANGE: r"\.\.\.",
    OPENGLOBAL: r"::{",
    CLOSEGLOBAL: r"}"
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
    def __init__(self, parts):
        self.parts = parts

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

class NamespaceDefinitionPrimitive:
    def __init__(self, name, body):
        self.name = name
        self.body = body

class NamespaceAccessPrimitive:
    def __init__(self, namespace, identifier):
        self.namespace = namespace
        self.identifier = identifier

class ReturnPrimitive:
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

class Graveyard:
    def __init__(self):
        self.source = ""
        self.tokens = []
        self.position = 0
        self.current = 0
        self.primitives = []
        self.monolith = [{}]

    def load(self, path):
        if path.split(".")[-1] != "graveyard":
            raise ValueError(f"{os.path.basename(path)} is not graveyard source code, please use .graveyard extension")
        else:
            with open(path, "r", encoding="utf-8") as file:
                self.source = file.read()

    def entry(self):
        self.source = self.source.lstrip().rstrip()
        entry_token = self.source[:3]
        exit_token = self.source[-1:]
        if entry_token == TOKEN_TYPES[OPENGLOBAL] and exit_token == TOKEN_TYPES[CLOSEGLOBAL]:
            self.source = self.source[3:-1]
            return
        else:
            raise SyntaxError("Global namespace not declared")

    def pretokenize(self):
        position = 0
        cleaned_source = []
        
        while position < len(self.source):
            match = None
            for token_type, pattern in [(SINGLELINECOMMENT, TOKEN_TYPES[SINGLELINECOMMENT]),
                                        (MULTILINECOMMENT, TOKEN_TYPES[MULTILINECOMMENT])]:
                regex_flags = re.DOTALL if token_type == MULTILINECOMMENT else re.MULTILINE
                regex = re.compile(pattern, regex_flags)
                match = regex.match(self.source, position)
                if match:
                    position = match.end()
                    break
            
            if not match:
                cleaned_source.append(self.source[position])
                position += 1
        
        self.source = ''.join(cleaned_source)

        self.top_level_library_sources = {}
        path_regex = re.compile(TOKEN_TYPES[PATH])

        for match in path_regex.finditer(self.source):
            full_token = match.group()
            normalized_token = full_token.replace("\\", "/")
                            
            if normalized_token not in self.top_level_library_sources:
                library_source = self.pretokenize_library(self.load_library_source(normalized_token[1:-1]))
                self.top_level_library_sources[normalized_token] = library_source

    def pretokenize_library(self, library_source):
        position = 0
        cleaned_source = []

        while position < len(library_source):
            match = None
            for token_type, pattern in [(SINGLELINECOMMENT, TOKEN_TYPES[SINGLELINECOMMENT]),
                                        (MULTILINECOMMENT, TOKEN_TYPES[MULTILINECOMMENT])]:
                regex_flags = re.DOTALL if token_type == MULTILINECOMMENT else re.MULTILINE
                regex = re.compile(pattern, regex_flags)
                match = regex.match(library_source, position)
                if match:
                    position = match.end()
                    break

            if not match:
                cleaned_source.append(library_source[position])
                position += 1

        pretokenized_library = ''.join(cleaned_source)

        path_regex = re.compile(TOKEN_TYPES[PATH])
        for match in path_regex.finditer(pretokenized_library):
            full_token = match.group()
            normalized_token = full_token.replace("\\", "/")
            nested_library_source = self.pretokenize_library(self.load_library_source(normalized_token[1:-1]))
            pretokenized_library = pretokenized_library.replace(full_token, nested_library_source)

        return pretokenized_library

    def load_library_source(self, path):
        library_path = f"{path}.graveyard"
        if not os.path.exists(library_path):
            raise ReferenceError(f"Library not found: {library_path}")
        with open(library_path, "r") as file:
            return file.read()

    def ingest(self):
        for import_statement, library_code in self.top_level_library_sources.items():
            self.source = self.source.replace(import_statement, library_code)
        self.top_level_library_sources.clear()

    def tokenize(self):
        tokens = []
        while self.position < len(self.source):
            match = None
            
            if self.source[self.position] == TOKEN_TYPES[FORMATTEDSTRING]:
                self.tokenize_formatted_string(tokens)
                continue

            for token_type, pattern in TOKEN_TYPES.items():
                regex = re.compile(pattern)
                match = regex.match(self.source, self.position)

                if match:
                    if token_type != WHITESPACE:
                        tokens.append((token_type, match.group(0)))
                    self.position = match.end()
                    break
            
            if not match:
                raise SyntaxError(f"Unexpected character: {self.source[self.position]}")

        self.tokens = tokens

    def tokenize_formatted_string(self, tokens):
        self.position += 1
        string_buffer = ""

        while self.position < len(self.source):
            char = self.source[self.position]

            if char == "{":
                # If no prior text, add an empty FORMATTEDSTRING token
                if not string_buffer:
                    tokens.append((FORMATTEDSTRING, ""))
                else:
                    tokens.append((FORMATTEDSTRING, string_buffer))
                    string_buffer = ""

                tokens.append((LEFTBRACE, "{"))
                self.position += 1
                self.tokenize_expression(tokens)
                continue

            elif char == "}":
                tokens.append((RIGHTBRACE, "}"))
                self.position += 1

            elif char == TOKEN_TYPES[FORMATTEDSTRING]:  # Closing formatted string
                if string_buffer:
                    tokens.append((FORMATTEDSTRING, string_buffer))
                self.position += 1
                return

            else:
                string_buffer += char
                self.position += 1

        raise SyntaxError("Unterminated formatted string")
    
    def tokenize_expression(self, tokens):
        """Temporarily switches to normal tokenization mode for expressions inside `{}`."""
        while self.position < len(self.source):
            char = self.source[self.position]

            if char == "}":
                return

            match = None
            for token_type, pattern in TOKEN_TYPES.items():
                regex_flags = re.DOTALL if token_type == MULTILINECOMMENT else re.MULTILINE
                regex = re.compile(pattern, regex_flags)
                match = regex.match(self.source, self.position)

                if match:
                    if token_type in {SINGLELINECOMMENT, MULTILINECOMMENT}:
                        self.position = match.end()
                        break
                    elif token_type != WHITESPACE:
                        tokens.append((token_type, match.group(0)))
                    self.position = match.end()
                    break

            if not match:
                raise SyntaxError(f"Unexpected character in formatted string expression: {self.source[self.position]}")

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
        elif self.match(NAMESPACE):
            if self.predict()[0] == IDENTIFIER and self.predict(2)[0] == LEFTBRACE:
                return self.parse_namespace_declaration()
        elif self.match(RETURN):
            statement = self.parse_return()
            self.consume(SEMICOLON)
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

    def parse_return(self):
        self.consume(RETURN)
        value = self.parse_or()
        return ReturnPrimitive(value)

    def parse_namespace_declaration(self):
        self.consume(NAMESPACE)
        name = self.consume(IDENTIFIER)
        self.consume(LEFTBRACE)
        
        body = []
        while not self.match(RIGHTBRACE):
            body.append(self.parse_statement())
        
        self.consume(RIGHTBRACE)
        return NamespaceDefinitionPrimitive(name, body)
    
    def parse_namespace_access(self):
        self.consume(NAMESPACE)
        namespace = self.consume(IDENTIFIER)
        self.consume(REFERENCE)
        identifier = self.consume(IDENTIFIER)
        
        return NamespaceAccessPrimitive(namespace, identifier)

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
        
        while not self.match(RIGHTBRACE):
            body.append(self.parse_statement())  # Treat return as a normal statement
        
        self.consume(RIGHTBRACE)
        return FunctionDefinitionPrimitive(name, parameters, body)

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

    def parse_formatted_string(self):
        parts = []

        while self.match(FORMATTEDSTRING) or self.match(LEFTBRACE):
            if self.match(FORMATTEDSTRING):
                parts.append(self.consume(FORMATTEDSTRING))
            elif self.match(LEFTBRACE):
                self.consume(LEFTBRACE)
                parts.append(self.parse_or())
                self.consume(RIGHTBRACE)

        return FormattedStringPrimitive(parts)

    def parse_numbers_parentheses(self):
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
            return self.parse_formatted_string()       
        elif self.match(IDENTIFIER):
            if self.predict()[0] == LEFTBRACKET:
                return self.parse_array_lookup()
            elif self.predict()[0] == REFERENCE:
                return self.parse_hashtable_lookup()
            elif self.predict()[0] == LEFTPARENTHESES:
                return self.parse_function_call()
            return IdentifierPrimitive(self.consume(IDENTIFIER))
        
        #@!
        elif self.match(SUBTRACTION):
            self.consume(SUBTRACTION)
            return UnaryOperationPrimitive("-", self.parse_numbers_parentheses())
            
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
        elif self.match(NAMESPACE):
            if self.predict()[0] == IDENTIFIER and self.predict(2)[0] == REFERENCE:
                return self.parse_namespace_access()
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
            RangePrimitive: lambda p: self.execute_range(p),
            NamespaceDefinitionPrimitive: lambda p: self.execute_namespace_declaration(p),
            NamespaceAccessPrimitive: lambda p: self.execute_namespace_access(p),
            ReturnPrimitive: lambda p: p
        }

        primitive_type = type(primitive)
        if primitive_type in execute_map:
            result = execute_map[primitive_type](primitive)
            # if isinstance(result, ReturnPrimitive): # don't think we need this here
            #     return result

            return result

        raise ValueError(f"Unknown primitive type: {primitive_type}")

    def execute_namespace_declaration(self, primitive):
        namespace_name = primitive.name

        # Find or create namespace in the global scope (bottom of the stack)
        global_scope = self.monolith[0]
        if namespace_name not in global_scope:
            global_scope[namespace_name] = {}

        # Push the namespace scope onto the stack
        self.monolith.append(global_scope[namespace_name])

        # Execute the body inside the new namespace scope
        for statement in primitive.body:
            self.execute(statement)

        # Pop the namespace scope off the stack after execution
        self.monolith.pop()

    def execute_namespace_access(self, primitive):
        namespace_name = primitive.namespace
        identifier = primitive.identifier

        # Look for the namespace in the global scope (bottom of the stack)
        global_scope = self.monolith[0]
        if namespace_name in global_scope and identifier in global_scope[namespace_name]:
            return global_scope[namespace_name][identifier]
        
        raise NameError(f"'{identifier}' not found in namespace '{namespace_name}'")

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
        hashtable = None

        # Search through the stack from the most recent scope to the global scope
        for scope in reversed(self.monolith):
            if primitive.identifier in scope:
                hashtable = scope[primitive.identifier]
                break

        if hashtable is None:
            raise NameError(f"Hashtable '{primitive.identifier}' is not defined")

        if not isinstance(hashtable, dict):
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
        return [self.execute(arg) for arg in args]
    
    def execute_cast_hashtable(self, args):
        return {self.execute(arg): None for arg in args}
    
    def execute_stoa(self, args):
        return list(self.execute(args[0]))
    
    def execute_reverse(self, args):
        values = []
        if isinstance(args, list):
            values = self.execute(args[0])
        return values[::-1]

    def execute_print(self, args):
        values = [self.execute(arg) for arg in args]
        print(*values)
        return True

    def execute_mod(self, args):
        dividend = self.execute(args[0])
        divisor = self.execute(args[1])
        return dividend % divisor
    
    def execute_floordiv(self, args):
        dividend = self.execute(args[0])
        divisor = self.execute(args[1])
        return dividend // divisor
    
    def execute_type(self, args):
        types = []
        for item in args:
            value = self.execute(item)
            if value is None:
                types.append("null")
            elif isinstance(value, bool):
                types.append("boolean")
            elif isinstance(value, int):
                types.append("integer")
            elif isinstance(value, float):
                types.append("float")
            elif isinstance(value, str):
                types.append("string")
            elif isinstance(value, list):
                types.append("array")
            elif isinstance(value, dict):
                types.append("hashtable")
            
        if not types:
            return None
        return types[0] if len(types) == 1 else types
    
    def execute_hello(self):
        print("hello world!")
        return True

    def execute_magic_number(self):
        return random.randint(10000000, 99999999)

    def execute_magic_weight(self):
        return round(random.random(), 8)
    
    def execute_magic_uid(self):
        return str(hex(random.randint(286331153, 4294967295)))[2:]
    
    def execute_magic_string(self):
        characters = "!#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~"
        return "".join(random.choice(characters) for i in range(16))
    
    def execute_magic_time(self):
        return time.time()

    def execute_magic_date_time(self):
        return datetime.now().strftime("%Y-%m-%d %H:%M:%S")

    def execute_fread(self, args):
        path = self.execute(args[0])
        if os.path.exists(path):
            with open(path, "r", encoding="utf-8") as file:
                data = file.read()
            return data
        else:
            raise ValueError(f"{path} is not a valid file path")

    def execute_fwrite(self, args):
        data = self.execute(args[0])
        path = self.execute(args[1])
        with open(path, "w") as file:
            file.write(data)

    def execute_function_call(self, primitive):
        builtins = {
            "b": lambda args: self.execute_cast_boolean(args),
            "i": lambda args: self.execute_cast_integer(args),
            "f": lambda args: self.execute_cast_float(args),
            "s": lambda args: self.execute_cast_string(args),
            "a": lambda args: self.execute_cast_array(args),
            "h": lambda args: self.execute_cast_hashtable(args),
            "stoa": lambda args: self.execute_stoa(args),
            "reverse": lambda args: self.execute_reverse(args),
            "print": lambda args: self.execute_print(args),
            "type": lambda args: self.execute_type(args),
            "hello": lambda *args: self.execute_hello(),
            "mod": lambda args: self.execute_mod(args),
            "floordiv": lambda args: self.execute_floordiv(args),
            "magic_number": lambda *args: self.execute_magic_number(),
            "magic_weight": lambda *args: self.execute_magic_weight(),
            "magic_uid": lambda *args: self.execute_magic_uid(),
            "magic_string": lambda *args: self.execute_magic_string(),
            "magic_time": lambda *args: self.execute_magic_time(),
            "magic_date_time": lambda *args: self.execute_magic_date_time(),
            "fread": lambda args: self.execute_fread(args),
            "fwrite": lambda args: self.execute_fwrite(args),
        }

        if primitive.name in builtins:
            return builtins[primitive.name](primitive.arguments)

        elif primitive.name in self.monolith[0]:
            function = self.monolith[0][primitive.name]

            if len(function.parameters) != len(primitive.arguments):
                raise ValueError(f"Incorrect number of arguments for function {primitive.name}")

            self.push_scope()

            try:
                # Bind arguments to parameters
                for parameter, argument in zip(function.parameters, primitive.arguments):
                    self.monolith[-1][parameter] = self.execute(argument)

                for statement in function.body:
                    result = self.execute(statement)

                    # If a return is encountered, return immediately
                    if isinstance(result, ReturnPrimitive):
                        return self.execute(result.value)

                return None  # Implicit return if no return statement was encountered

            finally:
                self.pop_scope()

        else:
            raise ValueError(f"Unknown function: {primitive.name}")

    def execute_assignment(self, primitive):
        value = self.execute(primitive.value)

        for scope in reversed(self.monolith):
            if primitive.identifier in scope:
                scope[primitive.identifier] = value
                return value

        self.monolith[-1][primitive.identifier] = value
        return value


    def execute_addition_assignment(self, primitive):
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

        scope_to_update[primitive.identifier] = new_value
        return new_value

    def execute_multiplication_assignment(self, primitive):
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

        scope_to_update[primitive.identifier] = new_value
        return new_value

    def execute_division_assignment(self, primitive):
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

        scope_to_update[primitive.identifier] = new_value
        return new_value

    def execute_exponentiation_assignment(self, primitive):
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
        if primitive.op in {"++", "--"}:
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

        # Handle negation and other unary operations
        operand = self.execute(primitive.right)

        operations = {
            "!": lambda x: not x,
            "-": lambda x: -x if isinstance(x, (int, float)) else TypeError(f"Cannot negate non-numeric type {type(x)}")
        }

        operation = operations.get(primitive.op)

        if operation is None:
            raise ValueError(f"Unknown operator: {primitive.op}")

        return operation(operand)

    def execute_formatted_string(self, primitive):
        evaluated_parts = []

        for part in primitive.parts:
            if type(part) == str:
                evaluated_parts.append(part)
            else:
                evaluated_parts.append(str(self.execute(part)))

        return "".join(evaluated_parts)
    
    def execute_array(self, primitive):
        return [self.execute(element) for element in primitive.elements]
    
    def execute_array_lookup(self, primitive):
        array = None
        
        for scope in reversed(self.monolith):
            if primitive.identifier in scope:
                array = scope[primitive.identifier]
                break

        if array is None:
            raise NameError(f"Array '{primitive.identifier}' is not defined")

        if not isinstance(array, list):
            raise TypeError(f"Variable {primitive.identifier} is not an array")

        index = self.execute(primitive.index)

        if not isinstance(index, int):
            raise TypeError("Array indices must be integers")

        return array[index]
    
    def execute_array_assignment(self, primitive):
        array = None

        for scope in reversed(self.monolith):
            if primitive.identifier in scope:
                array = scope[primitive.identifier]
                break

        if array is None:
            raise NameError(f"Array '{primitive.identifier}' is not defined")

        if not isinstance(array, list):
            raise TypeError(f"Variable {primitive.identifier} is not an array")

        index = self.execute(primitive.index)
        value = self.execute(primitive.value)

        if not isinstance(index, int):
            raise TypeError("Array indices must be integers")

        array[index] = value

    def execute_hashtable_assignment(self, primitive):
        hashtable = None

        # Search through the stack from the most recent scope to the global scope
        for scope in reversed(self.monolith):
            if primitive.identifier in scope:
                hashtable = scope[primitive.identifier]
                break

        if hashtable is None:
            raise NameError(f"Hashtable '{primitive.identifier}' is not defined")

        if not isinstance(hashtable, dict):
            raise TypeError(f"Variable {primitive.identifier} is not a hashtable")

        key = self.execute(primitive.key)

        if isinstance(key, (int, str)):
            value = self.execute(primitive.value)
        elif isinstance(key, float):
            raise TypeError("Hashtable keys cannot be float, must be integer or string")
        else:
            raise TypeError(f"Invalid hashtable key type: {type(key).__name__}")

        hashtable[key] = value

    def execute_array_append(self, primitive):
        array = None

        for scope in reversed(self.monolith):
            if primitive.identifier in scope:
                array = scope[primitive.identifier]
                break

        if array is None:
            raise NameError(f"Array '{primitive.identifier}' is not defined")

        if not isinstance(array, list):
            raise TypeError(f"'{primitive.identifier}' is not an array")

        value = self.execute(primitive.value)
        array.append(value)

    def execute_if_statement(self, primitive):
        # Loop through the condition blocks (if/else if)
        for condition, body in primitive.condition_blocks:
            if self.execute(condition):  # If the condition is True
                self.push_scope()
                try:
                    for statement in body:
                        result = self.execute(statement)
                        # If we encounter a return statement, propagate it upwards
                        if isinstance(result, ReturnPrimitive):
                            return result
                finally:
                    self.pop_scope()
                return  # Exit after the first block is executed

        # Execute the else block if no condition matched
        if primitive.else_body:
            self.push_scope()
            try:
                for statement in primitive.else_body:
                    result = self.execute(statement)
                    # If we encounter a return statement, propagate it upwards
                    if isinstance(result, ReturnPrimitive):
                        return result
            finally:
                self.pop_scope()

    def execute_while_statement(self, primitive):
        while self.execute(primitive.condition):
            self.push_scope()
            try:
                for statement in primitive.body:
                    result = self.execute(statement)
                    # If we encounter a return, propagate it upwards
                    if isinstance(result, ReturnPrimitive):
                        return result
            except ContinueException:
                continue
            except BreakException:
                break
            finally:
                self.pop_scope()

    def execute_for_statement(self, primitive):
        iterator_name = primitive.iterator
        limit = self.execute(primitive.limit)

        # Handle iteration through a dictionary
        if type(limit) == dict:
            for key in limit.keys():
                self.push_scope()
                try:
                    self.monolith[-1][iterator_name] = key

                    for statement in primitive.body:
                        result = self.execute(statement)
                        # If we encounter a return, propagate it upwards
                        if isinstance(result, ReturnPrimitive):
                            return result

                finally:
                    self.pop_scope()

        # Handle iteration through a list
        elif type(limit) == list:
            for item in limit:
                self.push_scope()
                try:
                    self.monolith[-1][iterator_name] = item

                    for statement in primitive.body:
                        result = self.execute(statement)
                        # If we encounter a return, propagate it upwards
                        if isinstance(result, ReturnPrimitive):
                            return result

                finally:
                    self.pop_scope()

        # Handle iteration through a range of integers
        else:
            if type(limit) != int:
                raise TypeError(f"Cannot iterate through {type(limit)} range")

            for i in range(limit):
                self.push_scope()
                try:
                    self.monolith[-1][iterator_name] = i

                    for statement in primitive.body:
                        result = self.execute(statement)
                        # If we encounter a return, propagate it upwards
                        if isinstance(result, ReturnPrimitive):
                            return result

                finally:
                    self.pop_scope()

    def execute_continue(self, primitive):
        raise ContinueException()

    def execute_break(self, primitive):
        raise BreakException()

#-------------------------------------------------------------------------------
#---- useful stuff for debugging ---

def print_primitive_tree(primitives):
    for primitive in primitives:
        print_primitive(primitive)

def print_primitive(node, indent=0):
    if node is None:
        return
    prefix = " " * (indent * 4) + "|-- "
    node_type = type(node).__name__
    if isinstance(node, IdentifierPrimitive):
        print(f"{prefix}{node_type} (name: {node.name})")
    elif isinstance(node, BooleanPrimitive):
        print(f"{prefix}{node_type} (value: {node.value})")
    elif isinstance(node, NumberPrimitive):
        print(f"{prefix}{node_type} (value: {node.value})")
    elif isinstance(node, StringPrimitive):
        print(f"{prefix}{node_type} (value: \"{node.value}\")")
    elif isinstance(node, NullPrimitive):
        print(f"{prefix}{node_type}")
    elif isinstance(node, BinaryOperationPrimitive):
        print(f"{prefix}{node_type} (op: {node.op})")
        print_primitive(node.left, indent + 1)
        print_primitive(node.right, indent + 1)
    elif isinstance(node, UnaryOperationPrimitive):
        print(f"{prefix}{node_type} (op: {node.op})")
        print_primitive(node.right, indent + 1)
    elif isinstance(node, AssignmentPrimitive):
        print(f"{prefix}{node_type}")
        print_primitive(node.identifier, indent + 1)
        print_primitive(node.value, indent + 1)
    elif isinstance(node, FunctionDefinitionPrimitive):
        print(f"{prefix}{node_type} (name: {node.name}, params: {', '.join(node.parameters)})")
        for stmt in node.body:
            print_primitive(stmt, indent + 1)
    elif isinstance(node, FunctionCallPrimitive):
        print(f"{prefix}{node_type} (name: {node.name})")
        for arg in node.arguments:
            print_primitive(arg, indent + 1)
    elif isinstance(node, IfStatementPrimitive):
        print(f"{prefix}{node_type}")
        for condition, body in node.condition_blocks:
            print(f"{' ' * (indent * 4 + 4)}|-- Condition:")
            print_primitive(condition, indent + 2)
            print(f"{' ' * (indent * 4 + 4)}|-- Body:")
            for stmt in body:
                print_primitive(stmt, indent + 2)
        if node.else_body:
            print(f"{prefix}    |-- Else:")
            for stmt in node.else_body:
                print_primitive(stmt, indent + 2)
    elif isinstance(node, WhileStatementPrimitive):
        print(f"{prefix}{node_type}")
        print_primitive(node.condition, indent + 1)
        for stmt in node.body:
            print_primitive(stmt, indent + 1)
    elif isinstance(node, ForStatementPrimitive):
        print(f"{prefix}{node_type} (iterator: {node.iterator})")
        print_primitive(node.limit, indent + 1)
        for stmt in node.body:
            print_primitive(stmt, indent + 1)
    elif isinstance(node, FormattedStringPrimitive):
        print(f"{prefix}{node_type}")
        for part in node.parts:
            print_primitive(part, indent + 1)
    elif isinstance(node, ArrayPrimitive):
        print(f"{prefix}{node_type}")
        for element in node.elements:
            print_primitive(element, indent + 1)
    elif isinstance(node, ArrayLookupPrimitive):
        print(f"{prefix}{node_type}")
        print_primitive(node.identifier, indent + 1)
        print_primitive(node.index, indent + 1)
    elif isinstance(node, ArrayAssignmentPrimitive):
        print(f"{prefix}{node_type}")
        print_primitive(node.identifier, indent + 1)
        print_primitive(node.index, indent + 1)
        print_primitive(node.value, indent + 1)
    elif isinstance(node, ArrayAppendPrimitive):
        print(f"{prefix}{node_type}")
        print_primitive(node.identifier, indent + 1)
        print_primitive(node.value, indent + 1)
    elif isinstance(node, (AdditionAssignmentPrimitive, SubtractionAssignmentPrimitive, 
                        MultiplicationAssignmentPrimitive, DivisionAssignmentPrimitive, 
                        ExponentiationAssignmentPrimitive)):
        print(f"{prefix}{node_type}")
        print_primitive(node.identifier, indent + 1)
        print_primitive(node.value, indent + 1)
    elif isinstance(node, HashtablePrimitive):
        print(f"{prefix}{node_type}")
        for key, value in node.value.items():
            print(f"{' ' * (indent * 4 + 4)}|-- Key: {key}")
            print_primitive(value, indent + 2)
    elif isinstance(node, HashtableLookupPrimitive):
        print(f"{prefix}{node_type}")
        print_primitive(node.identifier, indent + 1)
        print_primitive(node.key, indent + 1)
    elif isinstance(node, HashtableAssignmentPrimitive):
        print(f"{prefix}{node_type}")
        print_primitive(node.identifier, indent + 1)
        print_primitive(node.key, indent + 1)
        print_primitive(node.value, indent + 1)
    elif isinstance(node, RangePrimitive):
        print(f"{prefix}{node_type}")
        print_primitive(node.start, indent + 1)
        print_primitive(node.end, indent + 1)
    elif isinstance(node, NamespaceDefinitionPrimitive):
        print(f"{prefix}{node_type} (name: {node.name})")
        for stmt in node.body:
            print_primitive(stmt, indent + 1)
    elif isinstance(node, NamespaceAccessPrimitive):
        print(f"{prefix}{node_type}")
        print_primitive(node.namespace, indent + 1)
        print_primitive(node.identifier, indent + 1)
    elif isinstance(node, ContinuePrimitive):
        print(f"{prefix}{node_type}")
    elif isinstance(node, BreakPrimitive):
        print(f"{prefix}{node_type}")
    else:
        print(f"{prefix}literal or identifier: {node}")

def main():
    S = 899
    T = 900
    P = 901
    E = 902
    M = 903
    D = 904
    print("\n")

    graveyard = Graveyard()
    mode = E

    if mode == S:
        graveyard.load("C:\\Working\\graveyard\\working.graveyard")
        graveyard.entry()
        graveyard.pretokenize()
        graveyard.ingest()
        print(graveyard.source)
    elif mode == T:
        graveyard.load("C:\\Working\\graveyard\\working.graveyard")
        graveyard.entry()
        graveyard.pretokenize()
        graveyard.ingest()
        graveyard.tokenize()
        print(graveyard.tokens)
    elif mode == P:
        graveyard.load("C:\\Working\\graveyard\\working.graveyard")
        graveyard.entry()
        graveyard.pretokenize()
        graveyard.ingest()
        graveyard.tokenize()
        graveyard.parse()
        if len(graveyard.primitives) > 0:
            print_primitive_tree(graveyard.primitives)
    elif mode == E:
        graveyard.load("C:\\Working\\graveyard\\working.graveyard")
        graveyard.entry()
        graveyard.pretokenize()
        graveyard.ingest()
        graveyard.tokenize()
        graveyard.parse()
        graveyard.interpret()
    elif mode == M:
        graveyard.load("C:\\Working\\graveyard\\working.graveyard")
        graveyard.entry()
        graveyard.pretokenize()
        graveyard.ingest()
        graveyard.tokenize()
        graveyard.parse()
        graveyard.interpret()
        print(graveyard.monolith)
    elif mode == D:
        pass

if __name__ == "__main__":
    main()