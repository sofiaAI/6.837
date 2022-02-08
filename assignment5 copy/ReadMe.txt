• How do you compile and run your code? Specify which OS you have tested on.

I am on macOS and I use VSCode

• Did you collaborate with anyone in the class? If so, let us know who you talked to and what sort of help you gave or received.

No

• Were there any references (books, papers, websites, etc.) that you found particularly helpful for completing your assignment? Please provide a list. In particular, mention if you borrowed the model(s) used as your artifact from somewhere.
I did not use external references 

• Are there any known problems with your code? If so, please provide a list and, if possible, describe what you think the cause is and how you might fix them if you had more time or motivation. This is very important, as we’re much more likely to assign partial credit if you help us understand what’s going on.

No problems (that I know of)

• Did you do any of the extra credit? If so, let us know how to use the additional features. If there was a substantial amount of work involved, describe how you did it.

I did the PCF (which you should be able to tell is already implemented). To achieve this I sampled nearby pixels and averaged the shadow value. This shadow value was then factored into the light calculation.
I also did the spot light. To enable the spot light, set bool spot_light_enabled = true at the top of ShadowViewerApp::SetupScene(). To implement this, I made a new light class SpotLight. The light has a position, direction, and an angle within which light falls. From a fragment's position, I take a ray to the light and take the dot product with light direction. If the resulting value is less than the Spotlights range angle, the spot is lit. 

Finally, I have the majority of the implementation of screen-space ambient occlusion in phong.frag. While it is not fully implemented, all that is left too be set is some of the uniform variables, but I ran out of time. 


• Do you have any comments about this assignment that you’d like to share? We know this was a tough one, but did you learn a lot from it? Or was it overwhelming?

The timeline for this assignment was extremely unreasonable and tight. In addition, the office hours were of no use as there was far too many people. I started on this assignment earlier than any other assignment because I had no late days left. I even worked through thanksgiving. However, because of a bug that could not have been inferred from the slides or pset, I had to turn it in late. In addition, the assignment writeup was misleading. For example, it is written that phong.vert needs to be modified for shadow mapping, which was not true. Many of the key changes that needed to be made for the assignment to run are not even mentioned in the pset nor the slides. 

The problems with the due date were also mentioned on piazza, and while I understand that the due date cannot be moved, I think it would have been reasonable to allocate another late day as there were only 3 for the entire class.