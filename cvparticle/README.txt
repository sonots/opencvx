Examples of Configuration of Particle filtering applied 
for object tracking in video.

State and state transition model.

state1.h   - x,y,width,height,angle states and state transition model is
             next := current + noise
state2.h   - x,y,width,height,angle states and state transition model is
             next := current + speed + noise  
             Thus, states include previous values of x,y,.. too, totally 10.

Observation model depends on a state model.

observe1.h - template matching observation model.
             The reference image is taken at the first frame choosen by the user.
             The state model must have states x,y,width,height,angle.
             Both state1.h and state2.h is available for this.
observe2.h - PCA DIFS + DFFS likelihood mesurement observation model.
             PCA subspace must be trained or constructed beforehand.
             The state model must have states x,y,width,height,angle.
             Both state1.h and state2.h is available for this.
