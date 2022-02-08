• How do you compile and run your code? Specify which OS you have tested on.

I am on macOS and I use VSCode

• Did you collaborate with anyone in the class? If so, let us know who you talked to and what sort of help you gave or received.

No

• Were there any references (books, papers, websites, etc.) that you found particularly helpful for completing your assignment? Please provide a list. In particular, mention if you borrowed the model(s) used as your artifact from somewhere.
I did not use external references 

• Are there any known problems with your code? If so, please provide a list and, if possible, describe what you think the cause is and how you might fix them if you had more time or motivation. This is very important, as we’re much more likely to assign partial credit if you help us understand what’s going on.

The only issue is I don't have the lines. The reason why is because I have a single DrawLines Function which should be split into 2 functions, one to initialize the lines and one to update their positions. I would keep the line positions and index variables in the constructor instead, and Call the function to update line positions every Update cycle in Pendulum Node. The basics to Draw lines can be found in PendulumNode::DrawLines. This contains the necessary methods in variables but in practice the variables defining the line segment would be in the constructor and it would be initialized separately such that multiple children are not created.

• Did you do any of the extra credit? If so, let us know how to use the additional features. If there was a substantial amount of work involved, describe how you did it.

No


• Do you have any comments about this assignment that you’d like to share? We know this was a tough one, but did you learn a lot from it? Or was it overwhelming?

It was pretty reasonable
