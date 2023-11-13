from flask import Flask, render_template, request
import logging
import time
import subprocess

app = Flask(__name__)

# Configure logging
logging.basicConfig(filename='app.log', level=logging.INFO)

def run_custom_compiler(code):
    # For the purpose of this demonstration, we will simulate code execution
    # In a real-world scenario, consider using a safe execution environment
    logging.info("Executing code")
    start_time = time.time()
    
    # Simulated code execution (replace with actual execution logic)
    time.sleep(1)  # Simulating delay
    output = "Simulated Output: " + code
    
    execution_time = time.time() - start_time
    return output, execution_time

@app.route('/', methods=['GET', 'POST'])
def index():
    if request.method == 'POST':
        code = request.form['code']
        output, execution_time = run_custom_compiler(code)
        return render_template('index.html', code=code, output=output, execution_time=execution_time)
    return render_template('index.html', code='', output='', execution_time=0)

if __name__ == '__main__':
    app.run(debug=True)
