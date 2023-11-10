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

def selectExample(value):
    if value == "helloWorld":
        code = 'std::cout << "Hello World!";'
    elif value == 'simpleLoop':
        code = 'if (int i = 0; i < 10; i++) { std::cout << i << std::endl; }'
    elif value == 'bubbleSort':
        code = 'int arr[] = {2, 5, 1, 8, 5, 6, 3}; int n = sizeof(arr)/sizeof(int); int i, j; for (i = 0; i < n - 1; i++) { for (j = 0; j < n - i - 1; j++) { if (arr[j] > arr[j + 1]) { swap(arr[j], arr[j + 1]); } } }'
    elif value == 'findFactorial':
        code = 'int factorial(int n) { if (n > 1) { return n * factorial(n - 1) } else { return 1; }} int main() { int n = 10; cout << factorial(n); } }'
    return render_template('index.html', code=code, output=code)
            
if __name__ == '__main__':
    app.run(debug=True)
