from flask import Flask, request, jsonify
from flask_restx import Resource, Api
from flask_pymongo import PyMongo
from data.db_connect import get_mongo_uri
import subprocess
import logging
import werkzeug.exceptions as wz

app = Flask(__name__)
api = Api(app)

# Initialize MongoDB connection
app.config["MONGO_URI"] = get_mongo_uri()
mongo = PyMongo(app)

def inDatabase(programID):
    return mongo.db.sample_programs.fetch_one({id: programID})

@api.route('/tokenize')
class Tokenize(Resource):
    def post(self):
        try:
            code = request.json.get('code')
            output = run_compiler_stage(code, 'tokenize')
            return jsonify(output=output)
        except ValueError as e:
                raise wz.NotFound(f'{str(e)}')

@api.route('/parse')
class Parse(Resource):
    def post(self):
        try:
            code = request.json.get('code')
            output = run_compiler_stage(code, 'parse')
            return jsonify(output=output)
        except ValueError as e:
            raise wz.NotFound(f'{str(e)}')

@api.route('/evaluate')
class Evaluate(Resource):
    def post(self):
        try:
            code = request.json.get('code')
            output = run_compiler_stage(code, 'evaluate')
            return jsonify(output=output)
        except ValueError as e:
            raise wz.NotFound(f'{str(e)}')
        
@api.route('/sample_programs')
class SamplePrograms(Resource):
    def get(self):
        programs = mongo.db.sample_programs.find()
        return jsonify(programs=list(programs))
    
@api.route('/find_sample_program_by_ID')
class FindSampleProgramByID(Resource):
    def get(self, programID):
        program = mongo.db.sample_programs.find({id: programID})
        return jsonify(program)

@api.route('/delete_sample_program_by_ID')
class DeleteSampleProgramByID(Resource):
    def get(self, programID):
        try:
            return mongo.db.sample_programs.deleteOne({id: programID})
        except ValueError as e:
            raise wz.NotFound(f'{str(e)}')

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

