#ifndef TEXTURE_H
#define TEXTURE_H


class Texture
{
public:
    Texture(char *textureImage);
    unsigned int getId();

private:
    unsigned int id;
};

#endif // TEXTURE_H
