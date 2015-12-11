//////////////////////////////////////////////////////////////////////////////////////////
//
// default frament shader for solid colours
//

// inputs

varying vec3 model_pos_;
uniform float width;
uniform float height;

void main() { 

    //each iteration, it calculates: new = old*old + c, where c is a constant and old starts at current pixel
    float cRe = 0.0;
	float cIm = 0.0;                   //real and imaginary part of the constant c, determinate shape of the Julia Set
    float newRe = 0.0;
	float newIm = 0.0;
	float oldRe = 0.0;
	float oldIm = 0.0;   //real and imaginary parts of new and old
    float zoom = 1.0;
	float moveX = 0.0;
	float moveY = 0.0; //you can change these to zoom and change position
    vec3 color; //the RGB color value for the pixel
    int maxIterations = 300; //after how much iterations the function should stop

    //pick some values for the constant c, this determines the shape of the Julia Set
    cRe = -0.7;
    cIm = 0.27015;

	float x = model_pos_.x + width/2;
	float y = model_pos_.y + height/2;


	//calculate the initial real and imaginary part of z, based on the pixel location and zoom and position values
	newRe = 1.5 * (x - w / 2) / (0.5 * zoom * w) + moveX;
	newIm = (y - h / 2) / (0.5 * zoom * h) + moveY;
	//i will represent the number of iterations
	int i;
	//start the iteration process
	for(i = 0; i < maxIterations; i++)
	{
		//remember value of previous iteration
		oldRe = newRe;
		oldIm = newIm;
		//the actual iteration, the real and imaginary part are calculated
		newRe = oldRe * oldRe - oldIm * oldIm + cRe;
		newIm = 2 * oldRe * oldIm + cIm;
		//if the point is outside the circle with radius 2: stop
		if((newRe * newRe + newIm * newIm) > 4) break;
	}
	//use color model conversion to get rainbow palette, make brightness black if maxIterations reached
	color = hsv2rgb(vec3(i % 256, 255, 255 * (i < maxIterations)));
	//draw the pixel
	gl_FragColor = vec4(0.0, 1.0, 1.0, 1.0);

}
			
vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}
