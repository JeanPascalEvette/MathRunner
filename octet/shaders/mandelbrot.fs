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

//vec3 myRgbVec =  hsv2rgb(myHSVVec); this is to pass the value to the HSV to RGB function

void main() { 

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
	
	float zoom =  15.0;
	float moveX = 0.0;
	float moveY = -10.0;
	float c_im = (MaxIm - y*Im_factor + moveY) * (1.0/zoom);
	float c_re = (MinRe + x*Re_factor + moveX) * (1.0/zoom);

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
    //color = HSVtoRGB(ColorHSV(i % 256, 255, 255 * (i < MaxIterations)));

	vec3 myHSVVec = vec3((float(n)/256.0), 255.0, 255.0);
	vec3 myRgbVec =  hsv2rgb(myHSVVec);

	gl_FragColor = vec4(myRgbVec, 1.0);

	//gl_FragColor = vec4(float(n)/float(MaxIterations),float(n)/float(MaxIterations),float(n)/float(MaxIterations), 1.0);
	

	}
	else
	{ 
		gl_FragColor = vec4(0.0,0.0,0.0, 1.0);
	}
	

}
	
