from flask import Flask, request, redirect, url_for
from flask_restx import Api, Resource, fields
from flask_pymongo import PyMongo, ObjectId
import logging
import time
import subprocess
from data.db_connect import get_mongo_uri, connect_db

# Initialize Flask and Flask-RESTx
app = Flask(__name__)
api = Api(app, version='1.0', title='MonkeyIDE API', description='A simple API for MonkeyIDE')

# Configure MongoDB
app.config["MONGO_URI"] = get_mongo_uri()
mongo = PyMongo(app)

# Logging configuration
logging.basicConfig(filename='app.log', level=logging.INFO)

# Define namespace
ns = api.namespace('code', description='Code operations')

# Models for marshalling
code_model = api.model('Code', {
    'name': fields.String(required=True, description='The code name'),
    'code': fields.String(required=True, description='The source code')
})

compile_model = api.model('Compile', {
    'code': fields.String(required=True, description='Source code to be compiled and executed', example="1 + 1")
})

compile_response_model = api.model('CompileResponse', {
    'output': fields.String(description='Output of the compiled code'),
    'execution_time': fields.Float(description='Execution time in seconds')
})


# Define API endpoints
@ns.route('/db_content')
class CodeList(Resource):

#ENDPOINT #1 Listing all Sample Programs

    @ns.doc('list_codes', description='List all codes')  # Custom description for GET 
    @ns.marshal_list_with(code_model)
    def get(self):
        code_samples = mongo.db.samples.find()
        return list(code_samples), 200

#ENDPOINT #2 Insert a new sample rogram into Database

    @ns.doc('create_code', description='Create a new code')  # Custom description for POST
    @ns.expect(code_model)
    def post(self):
        code_data = request.json
        mongo.db.samples.insert_one(code_data)
        return {'result': 'code added'}, 201

@ns.route('/id_by_name')
class CodeIdByName(Resource):

#ENDPOINT #3 Get mongoDB objectID by sample program name

    @ns.doc('get_id_by_name')
    @ns.param('name', 'The name of the sample program')
    def get(self):
        name = request.args.get('name')
        if not name:
            api.abort(400, 'A program name is required')

        sample = mongo.db.samples.find_one({'name': name})
        if not sample:
            api.abort(404, 'Sample program not found')

        return str(sample['_id'])

@ns.route('/<string:sample_id>')
@ns.response(404, 'Sample not found')
@ns.param('sample_id', 'The sample identifier')
class Code(Resource):

#ENDPOINT #4 Get sample program name by MongoDB ObjectID

    @ns.doc('get_code', description='Get a specific code')  # Custom description for GET with ID
    @ns.marshal_with(code_model)
    def get(self, sample_id):
        try:
            oid = ObjectId(sample_id)
        except:
            api.abort(400, "Invalid ObjectId format")
        
        code_sample = mongo.db.samples.find_one({'_id': oid})
        if not code_sample:
            api.abort(404, 'Sample not found')
        return code_sample

#ENDPOINT #5 Delete sample program name by MongoDB ObjectID

    @ns.doc('delete_code', description='Delete a specific code')  # Custom description for DELETE
    def delete(self, sample_id):
        try:
            oid = ObjectId(sample_id)
            return {'result': 'code deleted'}, 204
        except:
            api.abort(400, "Invalid ObjectId format")

        result = mongo.db.samples.delete_one({'_id': oid})
        if result.deleted_count == 0:
            api.abort(404, 'Sample not found')
        return {'result': 'code deleted'}, 204

#ENDPOINT #6 Compile program using custom code input and print output

@ns.route('/compile')
class CompileCode(Resource):
    @ns.doc('compile_code')
    @ns.expect(compile_model)
    @ns.marshal_with(compile_response_model)
    def post(self):
        code = request.json.get('code')
        if not code:
            api.abort(400, "No code provided")

        output, execution_time = run_custom_compiler(code)
        return {'output': output, 'execution_time': execution_time}

#ENDPOINT #7: Get Total Number of Sample Programs

@ns.route('/total_samples')
class TotalSamples(Resource):
    @ns.doc('get_total_samples', description='Get total number of sample programs')
    def get(self):
        count = mongo.db.samples.count_documents({})
        return {'total_samples': count}

#ENDPOINT #8: Update a Sample Program by ID

@ns.route('/update/<string:sample_id>')
@ns.response(404, 'Sample not found')
@ns.param('sample_id', 'The sample identifier')
class UpdateCode(Resource):
    @ns.doc('update_code', description='Update a specific code')
    @ns.expect(code_model)
    def put(self, sample_id):
        try:
            oid = ObjectId(sample_id)
        except:
            api.abort(400, "Invalid ObjectId format")

        updated_data = request.json
        result = mongo.db.samples.update_one({'_id': oid}, {'$set': updated_data})
        
        if result.matched_count == 0:
            api.abort(404, 'Sample not found')
        
        return {'result': 'code updated'}, 200

#ENDPOINT #9: Get Sample Programs with a Specific Keyword

@ns.route('/search')
class SearchSamples(Resource):
    @ns.doc('search_samples', description='Search sample programs by keyword')
    @ns.param('keyword', 'Keyword to search for')
    def get(self):
        keyword = request.args.get('keyword', '')
        query = {"name": {"$regex": keyword, "$options": "i"}}  # Case-insensitive search
        samples = mongo.db.samples.find(query)

        # Convert ObjectId to string
        result = []
        for sample in samples:
            sample['_id'] = str(sample['_id'])  # Convert ObjectId to string
            result.append(sample)

        return result


#ENDPOINT #10: List Sample Programs by Page

@ns.route('/list_by_page')
class ListByPage(Resource):
    @ns.doc('list_by_page', description='List sample programs by page')
    @ns.param('page', 'Page number', _in='query', type='int')
    @ns.param('limit', 'Number of items per page', _in='query', type='int')
    def get(self):
        page = int(request.args.get('page', 1))
        limit = int(request.args.get('limit', 10))
        skip = (page - 1) * limit
        samples = mongo.db.samples.find().skip(skip).limit(limit)

        # Convert ObjectId to string
        result = []
        for sample in samples:
            sample['_id'] = str(sample['_id'])  # Convert ObjectId to string
            result.append(sample)

        return result


#ENDPOINT #11: Get Random Sample Program

import random

@ns.route('/random_sample')
class RandomSample(Resource):
    @ns.doc('get_random_sample', description='Get a random sample program')
    def get(self):
        count = mongo.db.samples.count_documents({})
        if count == 0:
            return {'message': 'No samples available'}, 404

        random_index = random.randint(0, count - 1)
        random_sample = mongo.db.samples.find().skip(random_index).limit(1)

        # Convert ObjectId to string
        result = []
        for sample in random_sample:
            sample['_id'] = str(sample['_id'])  # Convert ObjectId to string
            result.append(sample)

        return result[0] if result else {'message': 'Sample not found'}, 404

#ENDPOINT #12: Delete All Samples

@ns.route('/delete_all')
class DeleteAllSamples(Resource):
    @ns.doc('delete_all_samples', description='Delete all sample programs')
    def delete(self):
        mongo.db.samples.delete_many({})
        return {'result': 'all samples deleted'}, 204


# Helper function
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

if __name__ == '__main__':
    app.run(debug=True)