# Use a Python image that includes the necessary compilers
FROM python:3.8 as builder

# Install build essentials for compiling C++ code
RUN apt-get update && apt-get install -y \
    g++ \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory to /build
WORKDIR /build

# Copy your entire project to the container
COPY . .

# Compile the monkey_repl executable
RUN g++ -std=c++17 -I. -o monkey_repl \
    src/monkey/main.cpp \
    src/monkey/repl/repl.cpp \
    src/monkey/object/object.cpp \
    src/monkey/object/environment.cpp \
    src/monkey/lexer/lexer.cpp \
    src/monkey/parser/parser.cpp \
    src/monkey/evaluator/evaluator.cpp \
    src/monkey/ast/ast.cpp \
    src/monkey/token/token.cpp

# Runtime stage starts here
FROM python:3.8-slim

# Set the working directory in the container to /usr/src/app
WORKDIR /usr/src/app

# Copy the compiled monkey_repl from the builder stage
COPY --from=builder /build/monkey_repl .

# Make sure the executable has the right permissions
RUN chmod +x monkey_repl

# Copy only the necessary files for the Python environment
COPY src/python_interface/requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt
COPY src/python_interface/ .

# Expose the port the app runs on
EXPOSE 8000

# Set environment variables (adjust as necessary)
ENV DB_ENV=1
ENV MONGO_URI_CLOUD=mongodb+srv://yoxs_admin:9K668SonHpk9Oc2H@cluster0.jowbltk.mongodb.net/monkeyIDE

# Command to run the Flask application with Gunicorn
CMD ["gunicorn", "--bind", "0.0.0.0:8000", "endpoints:app"]
