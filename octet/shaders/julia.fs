//////////////////////////////////////////////////////////////////////////////////////////
//
// fragment shader to generate the Julia sets
//

// inputs

varying vec3 model_pos_;
uniform float width;
uniform float height;
uniform float zoom;
uniform float moveX;
uniform float moveY;
uniform float cRe;
uniform float cIm;
uniform int divisor;
		
//This uniform is used to choose the set of color displayed.
uniform vec4 divRGB;

//Utility function to convert color from the HSV format to the RGB format
vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(divRGB);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}


void main() { 
	int MaxIterations = 300;
	float x = model_pos_.x + width/2.0;  // xyz position of my square. Position relative to the middle of the section
	float y = model_pos_.y + height/2.0; // off set to put everything on 0,0


    
    //calculate the initial real and imaginary part of z, based on the pixel location and zoom and position values
    float newRe = 1.5 * (x - width / 2.0) / (0.5 * zoom * width) + moveX;
    float newIm = (y - height / 2.0) / (0.5 * zoom * height) + moveY;
    float oldRe = 0.0;
	float oldIm = 0.0;
	//n will represent the number of iterations
    int n;
    //start the iteration process
    for(n = 0; n < MaxIterations; n++)
    {
        //remember value of previous iteration
        oldRe = newRe;
        oldIm = newIm;
        //the actual iteration, the real and imaginary part are calculated
        newRe = oldRe * oldRe - oldIm * oldIm + cRe;
        newIm = 2.0 * oldRe * oldIm + cIm;
        //if the point is outside the circle with radius 2: stop
        if((newRe * newRe + newIm * newIm) > 4.0) break;
    }
       
	if(n < MaxIterations) 
	{ 
		vec3 myHSVVec = vec3(float(float(n)/float(divisor)), 0.99, 0.99);
		vec3 myRgbVec =  hsv2rgb(myHSVVec);
		gl_FragColor = vec4(myRgbVec, 1.0);
	}
	else
	{ 
		gl_FragColor = vec4(0.0,0.0,0.0, 1.0);
	}
}
	
