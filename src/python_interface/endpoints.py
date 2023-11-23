from flask import Flask, render_template, request, Blueprint
from flask_pymongo import PyMongo
import werkzeug.exceptions as wz

app = Flask(__name__)
app.config["MONGO_URI"] = "mongodb+srv://yoxs_admin:9K668SonHpk9Oc2H@cluster0.jowbltk.mongodb.net/monkeyIDE"
mongo = PyMongo(app)

USER_INPUT_ID = 'User_Input_ID'

def add_user_input(code: str) -> bool:
    _id = mongo.db.collection.insertOne(code)
    return _id is not None

#Implementing REST API
@app.route("/api/user_inputs", methods=['POST', 'GET'])
def user_inputs():
    """
    This class supports various operations,
    such as getting all user inputs and adding user inputs
    """
    if request.method == "POST":
        try:
            new_id = add_user_input(code)
            if new_id is None:
                raise wz.ServiceUnavailable('Technical Issue Encountered')
            return {USER_INPUT_ID: new_id}
        except ValueError as e:
            raise wz.NotAcceptable(f'{str(e)}')
        
    if request.method == "GET":
        return {
            'message': 'Return list of all user inputs'""", 
            'method': request.method"""
        }

