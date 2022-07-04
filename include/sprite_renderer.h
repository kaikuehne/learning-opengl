#include "HandmadeMath.h"
#include "shader.h"
#include "texture.h"

class SpriteRenderer {
	Shader shader;
	unsigned int quadVAO;

	void initRenderData();

public:
	SpriteRenderer(Shader shader);
	~SpriteRenderer();
	void draw(Texture &texture, hmm_vec2 position, hmm_vec2 size, float rotate = 0.0f, hmm_vec3 color = HMM_Vec3(1.0f, 1.0f, 1.0f));
};
