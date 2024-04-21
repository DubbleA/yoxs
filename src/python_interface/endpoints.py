from flask import Flask, request, redirect, url_for
from flask_cors import CORS
from flask_restx import Api, Resource, fields
from flask_pymongo import PyMongo, ObjectId
import logging
import time
import subprocess
from data.db_connect import get_mongo_uri, connect_db
import os


# Initialize Flask and Flask-RESTx
app = Flask(__name__)
CORS(app, resources={r"/*": {"origins": "*"}})
api = Api(app, version='1.0', title='MonkeyIDE API', description='A simple API for MonkeyIDE')

# Configure MongoDB
app.config["MONGO_URI"] = get_mongo_uri()
mongo = PyMongo(app)

# Secret Key for Authentication
app.config['SECRET_KEY'] = 'developer_secret_key_authentication'

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
HELLO_EP = '/hello'
HELLO_RESP = 'hello'

@ns.route(HELLO_EP)
class HelloWorld(Resource):
    """
    The purpose of the HelloWorld class is to have a simple test to see if the
    app is working at all.
    """
    def get(self):
        """
        A trivial endpoint to see if the server is running.
        It just answers with "hello world."
        """
        return {HELLO_RESP: 'world'}

# Define API endpoints
@ns.route('/db_content')
class CodeList(Resource):

#ENDPOINT #1 Listing all Sample Programs    
    """
    CodeList Endpoint

    This endpoint is used for operations related to code snippets stored in the database.
    It allows for listing all available code snippets and adding new ones.
    """
    @ns.doc('list_codes', description='List all codes')  # Custom description for GET 
    @ns.marshal_list_with(code_model)
    def get(self):
        """
        List All Codes

        Retrieves a list of all code snippets stored in the database.
        This can be used to display all available samples to the user.

        Responses:
            200: Success
            404: Not Found (if no codes are available)
        """
        code_samples = mongo.db.samples.find()
        return list(code_samples), 200

#ENDPOINT #2 Insert a new sample rogram into Database

    @ns.doc('create_code', description='Create a new code')  # Custom description for POST
    @ns.expect(code_model)
    def post(self):
        """
        Create a New Code

        Allows for adding a new code snippet to the database.
        The code data should be provided in the request body.

        Responses:
            201: Created
            400: Bad Request (if data is missing or invalid)
        """
        code_data = request.json
        mongo.db.samples.insert_one(code_data)
        return {'result': 'code added'}, 201

@ns.route('/id_by_name')
class CodeIdByName(Resource):
    """
    CodeIdByName Endpoint

    This endpoint retrieves the MongoDB ObjectID of a code sample based on its name.
    """
#ENDPOINT #3 Get mongoDB objectID by sample program name

    @ns.doc('get_id_by_name')
    @ns.param('name', 'The name of the sample program')
    def get(self):
        """
        Get ID by Name

        Fetches the MongoDB ObjectID for a given code sample name.

        Query Parameters:
            name: The name of the code sample.

        Responses:
            200: Success - Returns the ObjectID of the code sample.
            400: Bad Request - If the name parameter is missing.
            404: Not Found - If no code sample with the given name is found.
        """
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
    """
    Code Endpoint

    This endpoint allows fetching and deleting a specific code sample using its MongoDB ObjectID.
    """
#ENDPOINT #4 Get sample program name by MongoDB ObjectID

    @ns.doc('get_code', description='Get a specific code')  # Custom description for GET with ID
    @ns.marshal_with(code_model)
    def get(self, sample_id):
        """
        Get Code by ID

        Retrieves a specific code sample based on its MongoDB ObjectID.

        Path Parameters:
            sample_id: The MongoDB ObjectID of the code sample.

        Responses:
            200: Success - Returns the requested code sample.
            400: Bad Request - If the ObjectID is invalid.
            404: Not Found - If no code sample with the given ID is found.
        """
        try:
            oid = ObjectId(sample_id)
        except:
            api.abort(400, "Invalid ObjectId format")
        
        code_sample = mongo.db.samples.find_one({'_id': oid})
        if not code_sample:
            api.abort(404, 'Sample not found')
        return code_sample

#ENDPOINT #5 Delete sample program name by MongoDB ObjectID

    # @ns.doc('delete_code', description='Delete a specific code')  # Custom description for DELETE
    # def delete(self, sample_id):
    #     """
    #     Delete Code by ID

    #     Deletes a specific code sample based on its MongoDB ObjectID.

    #     Path Parameters:
    #         sample_id: The MongoDB ObjectID of the code sample to delete.

    #     Responses:
    #         204: No Content - Code sample successfully deleted.
    #         400: Bad Request - If the ObjectID is invalid.
    #         404: Not Found - If no code sample with the given ID is found.
    #     """
    #     try:
    #         oid = ObjectId(sample_id)
    #         return {'result': 'code deleted'}, 204
    #     except:
    #         api.abort(400, "Invalid ObjectId format")

    #     result = mongo.db.samples.delete_one({'_id': oid})
    #     if result.deleted_count == 0:
    #         api.abort(404, 'Sample not found')
    #     return {'result': 'code deleted'}, 204

#ENDPOINT #6 Compile program using custom code input and print output

@ns.route('/compile')
class CompileCode(Resource):
    """
    CompileCode Endpoint

    This endpoint compiles and executes the provided code snippet and returns the output.
    """
    @ns.doc('compile_code')
    @ns.expect(compile_model)
    @ns.marshal_with(compile_response_model)
    def post(self):
        """
        Compile and Execute Code

        Compiles and executes a provided code snippet. The source code must be included in the request body.

        Request Body:
            code: The source code to compile and execute.

        Responses:
            200: Success - Returns the output of the compiled code and execution time.
            400: Bad Request - If no code is provided or compilation fails.
        """
        code = request.json.get('code')
        if not code:
            api.abort(400, "No code provided")

        output, execution_time = run_custom_compiler(code)
        return {'output': output, 'execution_time': execution_time}

#ENDPOINT #7: Get Total Number of Sample Programs

@ns.route('/total_samples')
class TotalSamples(Resource):
    """
    TotalSamples Endpoint

    This endpoint returns the total number of code samples available in the database.
    """

    @ns.doc('get_total_samples', description='Get total number of sample programs')
    def get(self):
        """
        Get Total Samples

        Fetches the total count of code samples stored in the database.

        Responses:
            200: Success - Returns the total count of code samples.
        """
        count = mongo.db.samples.count_documents({})
        return {'total_samples': count}

#ENDPOINT #8: Update a Sample Program by ID

@ns.route('/update/<string:sample_id>')
@ns.response(404, 'Sample not found')
@ns.param('sample_id', 'The sample identifier')
class UpdateCode(Resource):
    """
    UpdateCode Endpoint

    This endpoint updates the details of a specific code sample identified by its MongoDB ObjectID.
    """

    @ns.doc('update_code', description='Update a specific code')
    @ns.expect(code_model)
    def put(self, sample_id):
        """
        Update Code by ID

        Modifies an existing code sample using its MongoDB ObjectID. The updated data should be provided in the request body.

        Path Parameters:
            sample_id: The MongoDB ObjectID of the code sample to update.

        Request Body:
            Updated details of the code sample.

        Responses:
            200: Success - Code sample successfully updated.
            400: Bad Request - If the ObjectID is invalid or data is missing.
            404: Not Found - If no code sample with the given ID is found.
        """
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
    """
    SearchSamples Endpoint

    This endpoint allows searching for code samples in the database using a keyword.
    """
    @ns.doc('search_samples', description='Search sample programs by keyword')
    @ns.param('keyword', 'Keyword to search for')
    def get(self):
        """
        Search Samples by Keyword

        Searches for code samples that match a given keyword in their names.

        Query Parameters:
            keyword: The keyword to search for in code sample names.

        Responses:
            200: Success - Returns a list of code samples matching the keyword.
        """
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
    """
    ListByPage Endpoint

    This endpoint lists code samples in a paginated format, allowing users to browse through samples in chunks.
    """
    @ns.doc('list_by_page', description='List sample programs by page')
    @ns.param('page', 'Page number', _in='query', type='int')
    @ns.param('limit', 'Number of items per page', _in='query', type='int')
    def get(self):
        """
        List Samples by Page

        Retrieves a specific page of code samples with a set limit on the number of items per page.

        Query Parameters:
            page: The page number to retrieve.
            limit: The maximum number of items per page.

        Responses:
            200: Success - Returns a paginated list of code samples.
        """
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
    """
    RandomSample Endpoint

    This endpoint fetches a random code sample from the database.
    """
    @ns.doc('get_random_sample', description='Get a random sample program')
    def get(self):
        """
        Get Random Sample

        Retrieves a random code sample from the database.

        Responses:
            200: Success - Returns a random code sample.
            404: Not Found - If no code samples are available.
        """
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

# @ns.route('/delete_all')
# class DeleteAllSamples(Resource):
#     """
#     DeleteAllSamples Endpoint

#     This endpoint deletes all code samples from the database.
#     """
#     @ns.doc('delete_all_samples', description='Delete all sample programs')
#     def delete(self):
#         """
#         Delete All Samples

#         Removes all code samples from the database.

#         Responses:
#             204: No Content - All code samples successfully deleted.
#         """
#         mongo.db.samples.delete_many({})
#         return {'result': 'all samples deleted'}, 204
from datetime import datetime

# New endpoint for getting the current server time
@ns.route('/server_time')
class ServerTime(Resource):
    @ns.doc('get_server_time', description='Get the current server time')
    def get(self):
        """
        Returns the current server time.
        """
        current_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        return {'server_time': current_time}

from functools import wraps
import jwt

#Function to check for valid API Keys
def token_required(f):
    @wraps(f)
    def decorated(*args, **kwargs):
        token = request.headers.get('Authorization')
        if not token:
            return {'message': 'Token is missing'}, 401
        
        try:
            data = jwt.decode(token, app.config['SECRET_KEY'], algorithms=["HS256"])
        except:
            return {'message': 'Token is invalid'}, 401
        return f(*args, **kwargs)
    return decorated

def generate_token():
    auth = request.authorization
    if not auth:
        return {'message': 'Invalid authorization'}, 401
    token = jwt.encode({'exp': datetime.datetime.utcnow() + datetime.timedelta(minutes=30)}, app.config['SECRET_KEY'], algorithm="HS256")
    return token.decode('UTF-8'), 200

# Developer endpoint
@ns.route("/dev_endpoint")
@token_required
class dev_endpoint(Resource):

    @ns.doc('dev_endpoint', description='Tool to assist developers')
    def get(self):
        """
        Logs that the endpoint was successfully accessed by a developer.
        """
        logging.info("Developer endpoint accessed")
        return "Success"

    
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
    # Use the PORT environment variable from Heroku, default to 5000 if not found
    port = int(os.environ.get('PORT', 5000))
    app.run(host='0.0.0.0', port=port, debug=True)