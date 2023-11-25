from flask import Flask, render_template, request
from flask_pymongo import PyMongo
import logging
import time
import subprocess

# Import connect_db from db_connect
from data.db_connect import get_mongo_uri

app = Flask(__name__)

# Initialize MongoDB connection
app.config["MONGO_URI"] = get_mongo_uri()
mongo = PyMongo(app)

# Configure logging
logging.basicConfig(filename='app.log', level=logging.INFO)

import subprocess

def run_custom_compiler(code):
    logging.info("Executing code")
    start_time = time.time()

    # Prepare the command to run your C++ program
    command = ['./monkey_repl']

    # Running the monkey_repl executable and passing the input
    process = subprocess.Popen(command, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    output, error = process.communicate(input=code)
    process.wait()

    execution_time = time.time() - start_time

    # Check for errors
    if process.returncode != 0:
        output = f"Error: {error}"

    return output, execution_time

@app.route('/', methods=['GET', 'POST'])
def index():
    if request.method == 'POST':
        code = request.form['code']

        output, execution_time = run_custom_compiler(code)
        return render_template('index.html', code=code, output=output, execution_time=execution_time)

    code_samples = mongo.db.samples.find()
    return render_template('index.html', code_samples=code_samples)

if __name__ == '__main__':
    app.run(debug=True)
