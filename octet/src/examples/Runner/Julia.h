namespace octet { // not used
	namespace shaders {
		class Julia_shader : public shader {
			GLuint modelToProjectionIndex_;
			GLuint colorIndex_;

		public:
			void init() {

				const char vertex_shader[] = SHADER_STR(
					attribute vec4 pos;
				uniform mat4 modelToProjection;

				void main() {
					gl_Position = modelToProjection * pos;
				}
				);

				const char fragment_shader[] = SHADER_STR(

					uniform vec4 color;

				void main() {

					//vec2 p = gl_FragCoord.xy / vec2(750, 750);
					//vec3 col = vec3(0.4, 0.8, 0.1) /** p.y*/;
					//gl_FragColor = color;

					screen(400, 300, 0, "Julia Set"); //make larger to see more detail!

													  //each iteration, it calculates: new = old*old + c, where c is a constant and old starts at current pixel
					double cRe, cIm;                   //real and imaginary part of the constant c, determinate shape of the Julia Set
					double newRe, newIm, oldRe, oldIm;   //real and imaginary parts of new and old
					double zoom = 1, moveX = 0, moveY = 0; //you can change these to zoom and change position
					ColorRGB color; //the RGB color value for the pixel
					int maxIterations = 300; //after how much iterations the function should stop

											 //pick some values for the constant c, this determines the shape of the Julia Set
					cRe = -0.7;
					cIm = 0.27015;

					//loop through every pixel
					for (int x = 0; x < w; x++)
						for (int y = 0; y < h; y++)
						{
							//calculate the initial real and imaginary part of z, based on the pixel location and zoom and position values
							newRe = 1.5 * (x - w / 2) / (0.5 * zoom * w) + moveX;
							newIm = (y - h / 2) / (0.5 * zoom * h) + moveY;
							//i will represent the number of iterations
							int i;
							//start the iteration process
							for (i = 0; i < maxIterations; i++)
							{
								//remember value of previous iteration
								oldRe = newRe;
								oldIm = newIm;
								//the actual iteration, the real and imaginary part are calculated
								newRe = oldRe * oldRe - oldIm * oldIm + cRe;
								newIm = 2 * oldRe * oldIm + cIm;
								//if the point is outside the circle with radius 2: stop
								if ((newRe * newRe + newIm * newIm) > 4) break;
							}
							//use color model conversion to get rainbow palette, make brightness black if maxIterations reached
							color = HSVtoRGB(ColorHSV(i % 256, 255, 255 * (i < maxIterations)));
							//draw the pixel
							pset(x, y, color);
						}
				}
				);

				shader::init(vertex_shader, fragment_shader);

				colorIndex_ = glGetUniformLocation(program(), "color");
				modelToProjectionIndex_ = glGetUniformLocation(program(), "modelToProjection");
			}

			void render(const mat4t &modelToProjection, const vec4 &color) {
				shader::render();
				glUniform4fv(colorIndex_, 1, color.get());
				glUniformMatrix4fv(modelToProjectionIndex_, 1, GL_FALSE, modelToProjection.get());
			}
		};
	}
}
