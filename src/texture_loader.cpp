#include "texture_loader.h"
#include "texture.h"

Texture *engine_loadTexture(const char *path) { return &ResourceManager::load_texture(path, path, false); }

void engine_disposeTexture(Texture *texture) {
    // FIXME: Maybe call stbi_image_free(data) here, not in ResourceManager.
}
