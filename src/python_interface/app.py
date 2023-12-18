from flask import Flask, render_template, request, redirect, url_for
from flask_pymongo import PyMongo
import logging
import time
import subprocess
import os
from data.db_connect import get_mongo_uri

app = Flask(__name__)

app.config["MONGO_URI"] = get_mongo_uri()
#app.config["MONGO_URI"] = os.environ.get("MONGO_URI_CLOUD")

mongo = PyMongo(app)

logging.basicConfig(filename='app.log', level=logging.INFO)

def run_custom_compiler(code):
    logging.info("Executing code")
    start_time = time.time()

    command = ['./monkey_repl']
    try:
        process = subprocess.Popen(command, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        output, error = process.communicate(input=code, timeout=10)  # Timeout added
        process.wait()

        if process.returncode != 0:
            output = f"Error: {error}"
    except subprocess.TimeoutExpired:
        output = "Execution timed out"
    except Exception as e:
        logging.error(f"Execution failed: {str(e)}")
        output = "An error occurred during execution"

    execution_time = time.time() - start_time
    return output, execution_time

@app.route('/', methods=['GET', 'POST'])
def index():
    if request.method == 'POST':
        code = request.form['code']
        output, execution_time = run_custom_compiler(code)
        return render_template('index.html', code=code, output=output, execution_time=execution_time)

    code_samples = mongo.db.samples.find()
    return render_template('index.html', code_samples=code_samples)

@app.route('/add_sample', methods=['POST'])
def add_sample():
    name = request.form.get('name')
    code = request.form.get('code')
    if name and code:
        mongo.db.samples.insert_one({'name': name, 'code': code})
    return redirect(url_for('index'))

@app.route('/remove_sample/<sample_id>', methods=['GET'])
def remove_sample(sample_id):
    mongo.db.samples.delete_one({'_id': sample_id})
    return redirect(url_for('index'))

if __name__ == '__main__':
    app.run(debug=True)
