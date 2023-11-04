from flask import Flask, render_template, request

app = Flask(__name__)

@app.route('/', methods=['GET', 'POST'])
def index():
    if request.method == 'POST':
        code = request.form['code']
        output = run_custom_compiler(code) 
        return render_template('index.html', code=code, output=output)
    return render_template('index.html', code='', output='')

def run_custom_compiler(code):
    # Dummy function
    return "Output" + code 
if __name__ == '__main__':
    app.run(debug=True)
