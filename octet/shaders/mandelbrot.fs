//////////////////////////////////////////////////////////////////////////////////////////
//
// fragment shader to generate the mandelbrot set
//

// inputs

varying vec3 model_pos_;
uniform float width;
uniform float height;
uniform float cRe;
uniform float cIm;
		
//Utility function to convert color from the HSV format to the RGB format
vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}


void main() { 
	float invertedIm = -cIm; //Inverting the Imaginary number else the set is rendered upside down
	int divisor = 200;
	float crossSize = 0.05;
	float crossWidth = 0.005;
	float crossWidthBlack = crossWidth * 2.2;
	float x = model_pos_.x + width/2.0;  // xyz position of my square. Position relative to the middle of the section
	float y = model_pos_.y + height/2.0; // off set to put everything on 0,0

    //each iteration, it calculates: new = old*old + c, where c is a constant and old starts at current pixel
    float MinRe = -2.0;
	float MaxRe = 1.0;
	float MinIm = -1.5;
	float MaxIm = MinIm+(MaxRe-MinRe) * height/width;
	float Re_factor = (MaxRe-MinRe) / (width-1.0);
	float Im_factor = (MaxIm-MinIm) / (height-1.0);
	int MaxIterations = 300;
	
	float zoom = 1.7;
	float c_im = (MaxIm - y*Im_factor + 0.2*zoom) * (1.0/zoom);
	float c_re = (MinRe + x*Re_factor - 0.5*zoom) * (1.0/zoom);


	//Creating black borders around minimap
	if(MinRe + x*Re_factor > 1.72 || MinRe + x*Re_factor < -1.97 || MaxIm - y*Im_factor > 1.48 || MaxIm - y*Im_factor < -2.22)
	{
		gl_FragColor = vec4(0.0,0.0,0.0,1.0);
		return;
	}
	
	//adding the white cross
	if((c_re < cRe + crossSize && c_re > cRe - crossSize && c_im < invertedIm + crossWidth && c_im > invertedIm - crossWidth)
		|| (c_im < invertedIm + crossSize && c_im > invertedIm - crossSize && c_re < cRe + crossWidth && c_re > cRe - crossWidth)){
		gl_FragColor = vec4(1.0,1.0,1.0,1.0);
		return;
	} //Adding black borders to cross
	else if((c_re < cRe + crossSize + crossWidth && c_re > cRe - crossSize - crossWidth && c_im < invertedIm + crossWidthBlack && c_im > invertedIm - crossWidthBlack)
		|| (c_im < invertedIm + crossSize  + crossWidth && c_im > invertedIm - crossSize - crossWidth && c_re < cRe + crossWidthBlack && c_re > cRe - crossWidthBlack)){
		gl_FragColor = vec4(0.0,0.0,0.0,1.0);
		return;
	}

	//Generating the set
	float Z_re = c_re, Z_im = c_im;
	bool isInside = true;
	int n;
	for(n=0; n<MaxIterations; ++n)
	{
		float Z_re2 = Z_re*Z_re;
		float Z_im2 = Z_im*Z_im;
		if(Z_re2 + Z_im2 > 4.0)
			break;
		Z_im = 2.0*Z_re*Z_im + c_im;
		Z_re = Z_re2 - Z_im2 + c_re;

		
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
	
