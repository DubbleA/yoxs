## Progress:

### API Server Setup:
1. Set up a back-end server using Flask or another suitable framework. This server will handle requests from the front-end and interact with the interpreter components.
2. Defined RESTful endpoints for sending code to be interpreted and retrieving the visualizations of the lexer, parser, and execution processes.

### Database Integration:
1. Utilized MongoDB or a similar database to store and manage code snippets, tokens, ASTs, and other relevant data.
2. Ensured secure and efficient data handling, with considerations for scalability and data integrity.
Interpreter Integration:
3. Integrated the C++ interpreter with the back-end server, ensuring seamless communication and data exchange.
4. Implemented necessary handlers and middleware for processing code and generating visualization data.

## Goals: 

### Visualization Dashboard Development:
1. Develop a front-end dashboard inspired by emu86 using a suitable framework.
2. Create interactive visualizations for the lexer, parser, and execution processes, allowing users to see how code gets processed in real-time.
3. Implement features for users to input code, view the resulting tokens, AST, and the final output.

### Deployment and Cloud Integration:
1. Host the application on a cloud platform like pythonanywhere to ensure accessibility and scalability.
2. Set up necessary cloud resources, like computing instances and databases.

### Testing and Optimization:
1. Conduct thorough testing to ensure the system works as expected, fixing any bugs or issues that arise.
2. Optimize the performance of both the back-end and front-end, ensuring a smooth user experience.

### Documentation and User Guide:
1. Create comprehensive documentation describing the architecture, endpoints, and how to use the visualizer dashboard.
2. Provide a user guide for navigating the dashboard and understanding the visualizations.
