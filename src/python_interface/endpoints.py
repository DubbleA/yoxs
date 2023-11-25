from flask import Flask, request, jsonify
from flask_restx import Resource, Api
from flask_pymongo import PyMongo
from data.db_connect import get_mongo_uri
import subprocess
import logging

app = Flask(__name__)
api = Api(app)

# Initialize MongoDB connection
app.config["MONGO_URI"] = get_mongo_uri()
mongo = PyMongo(app)

@api.route('/tokenize')
class Tokenize(Resource):
    def post(self):
        code = request.json.get('code')
        output = run_compiler_stage(code, 'tokenize')
        return jsonify(output=output)

@api.route('/parse')
class Parse(Resource):
    def post(self):
        code = request.json.get('code')
        output = run_compiler_stage(code, 'parse')
        return jsonify(output=output)

@api.route('/evaluate')
class Evaluate(Resource):
    def post(self):
        code = request.json.get('code')
        output = run_compiler_stage(code, 'evaluate')
        return jsonify(output=output)

@api.route('/sample_programs')
class SamplePrograms(Resource):
    def get(self):
        programs = mongo.db.sample_programs.find()
        return jsonify(programs=list(programs))

def run_compiler_stage(code, stage):
    logging.info(f"Running {stage} stage")
    command = [f'./monkey_{stage}']  # Adjust the executable names as needed
    process = subprocess.Popen(command, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    output, error = process.communicate(input=code)

    if process.returncode != 0:
        output = f"Error: {error}"

    return output

if __name__ == '__main__':
    app.run(debug=True)

