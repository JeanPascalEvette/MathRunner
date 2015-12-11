//////////////////////////////////////////////////////////////////////////////////////////
//
// default frament shader for solid colours
//

// inputs

varying vec3 model_pos_;
uniform float width;
uniform float height;
		
vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() { 

    //each iteration, it calculates: new = old*old + c, where c is a constant and old starts at current pixel
    float cRe = 0.0;
	float cIm = 0.0;                   //real and imaginary part of the constant c, determinate shape of the Julia Set
    float newRe = 0.0;
	float newIm = 0.0;
	float oldRe = 0.0;
	float oldIm = 0.0;   //real and imaginary parts of new and old
    float zoom = 1.0;
	float moveX = 3.0;
	float moveY = 3.0; //you can change these to zoom and change position
    int maxIterations = 300; //after how much iterations the function should stop

    //pick some values for the constant c, this determines the shape of the Julia Set
    cRe = -0.7;
    cIm = 0.27015;

	float x = model_pos_.x;
	float y = model_pos_.y;


	//calculate the initial real and imaginary part of z, based on the pixel location and zoom and position values
	newRe = 1.5 * (x) / (0.5 * zoom * width) + moveX;
	newIm = (y) / (0.5 * zoom * height) + moveY;
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
		newIm = 2.0 * oldRe * oldIm + cIm;
		//if the point is outside the circle with radius 2: stop
		if((newRe * newRe + newIm * newIm) > 4.0) break;
	}
	//use color model conversion to get rainbow palette, make brightness black if maxIterations reached
	float colorH = mod(float(i), 256.0)/360.0;
	float colorS = 255.0;
	float colorV = 255.0;
	if(i > maxIterations)
		colorV = 0.0;
	vec3 colorHSV = vec3(colorH, colorS, colorV);
	vec3 colorRGB = hsv2rgb(colorHSV);
	//draw the pixel
	gl_FragColor = vec4(colorRGB.xyz / 255.0, 1.0);

}
	
