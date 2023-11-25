import os
import pymongo as pm

# Constants for environment selection
LOCAL = "0"
CLOUD = "1"

# Database details
DB_NAME = 'monkeyIDE'
MONGO_ID = '_id'

client = None

def get_mongo_uri():
    if os.environ.get("DB_ENV", LOCAL) == CLOUD:
        mongo_uri = os.environ.get("MONGO_URI_CLOUD")
        if not mongo_uri:
            raise ValueError('You must set your cloud MongoDB URI in environment variables.')
    else:
        mongo_uri = os.environ.get("MONGO_URI_LOCAL", "mongodb://localhost:27017/local")

    return mongo_uri

def connect_db():
    """
    This provides a uniform way to connect to the DB across all uses.
    Returns a mongo client object... maybe we shouldn't?
    Also set global client variable.
    We should probably either return a client OR set a
    client global.
    """
    global client
    if client is None:
        if os.environ.get("DB_ENV", LOCAL) == CLOUD:
            mongo_uri = os.environ.get("MONGO_URI_CLOUD")
            if not mongo_uri:
                raise ValueError('You must set your cloud MongoDB URI in environment variables.')
            client = pm.MongoClient(mongo_uri)
        else:
            mongo_uri = os.environ.get("MONGO_URI_LOCAL", "mongodb://localhost:27017/local")
            client = pm.MongoClient(mongo_uri)

    return client[DB_NAME]


def insert_one(collection, doc, db=DB_NAME):
    """
    Insert a single doc into collection.
    """
    print(f'{db=}')
    return client[db][collection].insert_one(doc)


def fetch_one(collection, filt, db=DB_NAME):
    """
    Find with a filter and return on the first doc found.
    """
    for doc in client[db][collection].find(filt):
        if MONGO_ID in doc:
            # Convert mongo ID to a string so it works as JSON
            doc[MONGO_ID] = str(doc[MONGO_ID])
        return doc


def del_one(collection, filt, db=DB_NAME):
    """
    Find with a filter and return on the first doc found.
    """
    client[db][collection].delete_one(filt)


def fetch_all(collection, db=DB_NAME):
    ret = []
    for doc in client[db][collection].find():
        ret.append(doc)
    return ret


def fetch_all_as_dict(key, collection, db=DB_NAME):
    ret = {}
    for doc in client[db][collection].find():
        del doc[MONGO_ID]
        ret[doc[key]] = doc
    return ret