#!/bin/bash

# Navigate to your project directory
cd /home/ag7432/yoxs/src/python_interface

# Activate the virtual environment
source /home/ag7432/.virtualenvs/myvirtualenv/bin/activate

# Pull the latest changes from your repository
git pull

# Add any additional commands you need after updating your code
# For example, migrating database changes or collecting static files in Django
# python manage.py migrate
# python manage.py collectstatic --noinput

echo "Deployment completed successfully."
