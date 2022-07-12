#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include <spine/Extension.h>
#include <spine/spine.h>

#include "resource_manager.h"
#include "texture.h"

using namespace spine;

Texture *engine_loadTexture(const char *path);
void engine_disposeTexture(Texture *texture);

class MyTextureLoader : public TextureLoader {
  public:
    MyTextureLoader() {}

    ~MyTextureLoader() {}

    // Called when the atlas loads the texture of a page.
    void load(AtlasPage &page, const String &path) {
        Texture *texture = engine_loadTexture(path.buffer());

        // if texture loading failed, we simply return.
        if (!texture) {
            return;
        }

        // store the Texture on the rendererObject so we can
        // retrieve it later for rendering.
        page.setRendererObject(texture);

        // store the texture width and height on the spAtlasPage
        // so spine-c can calculate texture coordinates for
        // rendering.
        page.width = texture->width;
        page.height = texture->height;
    }

    // Called when the atlas is disposed and itself disposes its atlas pages.
    void unload(void *texture) {
        // the texture parameter is the texture we stored in the page via page->setRendererObject()
        engine_disposeTexture((Texture *)texture);
    }
};

#endif
