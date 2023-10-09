# REPL Documentation

## Introduction
The REPL, or Read-Evaluate-Print Loop, is a simple, interactive programming environment. Similar to what's found in languages like Python and Ruby, our REPL reads input, sends it to the interpreter for processing, and then loops back to await further input.

## Current Functionality

### Reading & Tokenization
At this stage, our REPL primarily focuses on reading input and processing it into its tokenized form. While the term "Evaluate" is part of the REPL acronym, we aren't evaluating our source code yet. The current emphasis lies on capturing the user's input and transforming it into a series of tokens that provide a structured representation of the code.

### Continuation Until EOF
Our REPL continues to read new input iteratively. This loop persists until the system encounters the designated "end of file" (EOF) token, signaling the REPL to terminate.

## Future Considerations
As we further develop our REPL, we will incorporate the evaluation stage, enabling users to not only input their code but also witness the results of their code execution in real-time. This dynamic nature, paired with immediate feedback, makes REPL a powerful tool for developers, aiding in rapid prototyping and debugging.

## Note
The tokens produced by the REPL are integral to the process, serving as the bridge between raw user input and the interpreter. For more details on the tokenization process and the structure of tokens, please refer to the **Token Documentation**.