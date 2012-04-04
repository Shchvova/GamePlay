#include "Base.h"
#include "Gamepad.h"
#include "Texture.h"

#define INVALID_CONTACT_INDEX ((unsigned int)-1)

Gamepad::Gamepad(const char* texturePath, unsigned int joysticks, unsigned int buttons)
    : _joystickCount(joysticks), _buttonCount(buttons), _texture(NULL), _spriteBatch(NULL)
{
    assert (texturePath);
    assert(joysticks <= _joystickCount);
    assert(buttons <= _buttonCount);
    
    // Load the gamepad texture.
    if (strlen(texturePath) == 0)
    {
        printError("Error loading Gamepad: No valid texture path specified, in %s", texturePath);
    }

    _texture = Texture::create(texturePath, false);
    assert(_texture);

    // TODO: determine the "actual" number of sprites required when parsing the .gamepad file.
    _spriteBatch = SpriteBatch::create(_texture, 0, _buttonCount + _joystickCount);

    _joysticks = new Joystick*[_joystickCount];
    for (unsigned int i = 0; i < _joystickCount; i++)
    {
        _joysticks[i] = new Joystick();
    }

    _buttons = new Button*[_buttonCount];
    for (unsigned int i = 0; i < _buttonCount; i++)
    {
        _buttons[i] = new Button();
    }

    for (unsigned int i = 0; i < MAX_TOUCH_INPUTS; i++)
    {
        _touches[i] = false;
    }
}

Gamepad::Gamepad(const Gamepad* g)
{
}

Gamepad::~Gamepad()
{
    SAFE_RELEASE(_texture);
    SAFE_DELETE(_spriteBatch);

    for (unsigned int i = 0; i < _joystickCount; i++)
    {
        delete _joysticks[i];
    }
    SAFE_DELETE_ARRAY(_joysticks);

    for (unsigned int i = 0; i < _buttonCount; i++)
    {
        delete _buttons[i];
    }
    SAFE_DELETE_ARRAY(_buttons);
}

Gamepad::Button::Button()
    : _pressed(BUTTON_RELEASED), _defaultTextureEnabled(false), _focusTextureEnabled(false), _contactIndex(INVALID_CONTACT_INDEX)
{
    
}

Gamepad::Button::Button(const Button& button)
{
}

Gamepad::Button::~Button()
{
}

Gamepad::Joystick::Joystick()
    : _displacement(Vector2::zero()), _direction(Vector2::zero()), _radius(0.0f),
    _defaultTextureInnerEnabled(false), _defaultTextureOuterEnabled(false), _contactIndex(INVALID_CONTACT_INDEX)
{
}

Gamepad::Joystick::Joystick(const Joystick& joystick)
{
}

Gamepad::Joystick::~Joystick()
{
}

void Gamepad::setButton(unsigned int buttonId, Rect* bounds, Rect* defaultTextureRegion, Rect* focusTextureRegion)
{
    assert(buttonId < _buttonCount);

    if (bounds)
    {
        _buttons[buttonId]->_region.x = bounds->x;
        _buttons[buttonId]->_region.y = bounds->y;
        _buttons[buttonId]->_region.width = bounds->width;
        _buttons[buttonId]->_region.height = bounds->height;
    }

    if (defaultTextureRegion)
    {
        _buttons[buttonId]->_defaultTexCoord.u1 = (defaultTextureRegion->x / _texture->getWidth());
        _buttons[buttonId]->_defaultTexCoord.v1 = 1.0f - (defaultTextureRegion->y / _texture->getHeight());
        _buttons[buttonId]->_defaultTexCoord.u2 = _buttons[buttonId]->_defaultTexCoord.u1 + (defaultTextureRegion->width / _texture->getWidth());
        _buttons[buttonId]->_defaultTexCoord.v2 = _buttons[buttonId]->_defaultTexCoord.v1 - (defaultTextureRegion->height / _texture->getHeight());
        _buttons[buttonId]->_defaultTextureEnabled = true;
    }

    if (focusTextureRegion)
    {
        _buttons[buttonId]->_focusTexCoord.u1 = (focusTextureRegion->x / _texture->getWidth());
        _buttons[buttonId]->_focusTexCoord.v1 = 1.0f - (focusTextureRegion->y / _texture->getHeight());
        _buttons[buttonId]->_focusTexCoord.u2 = _buttons[buttonId]->_focusTexCoord.u1 + (focusTextureRegion->width / _texture->getWidth());
        _buttons[buttonId]->_focusTexCoord.v2 = _buttons[buttonId]->_focusTexCoord.v1 - (focusTextureRegion->height / _texture->getHeight());
        _buttons[buttonId]->_focusTextureEnabled = true;
    }
}

Gamepad::ButtonState Gamepad::getButtonState(unsigned int index) const
{
    assert(index < _buttonCount);

    return _buttons[index]->_pressed;
}

void Gamepad::setJoystick(unsigned int joystickId, Rect* regionInner, Rect* textureRegionInner, Rect* regionOuter, Rect* textureRegionOuter, float radius)
{
    assert (joystickId < _joystickCount);

    _joysticks[joystickId]->_radius = radius;

    if (regionInner)
    {
        _joysticks[joystickId]->_regionInner.x = regionInner->x;
        _joysticks[joystickId]->_regionInner.y = regionInner->y;
        _joysticks[joystickId]->_regionInner.width = regionInner->width;
        _joysticks[joystickId]->_regionInner.height = regionInner->height;
    }

    if (textureRegionInner)
    {
        _joysticks[joystickId]->_defaultTexCoordInner.u1 = (textureRegionInner->x / _texture->getWidth());
        _joysticks[joystickId]->_defaultTexCoordInner.v1 = 1.0f - (textureRegionInner->y / _texture->getHeight());
        _joysticks[joystickId]->_defaultTexCoordInner.u2 = _joysticks[joystickId]->_defaultTexCoordInner.u1 + (textureRegionInner->width / _texture->getWidth());
        _joysticks[joystickId]->_defaultTexCoordInner.v2 = _joysticks[joystickId]->_defaultTexCoordInner.v1 - (textureRegionInner->height / _texture->getHeight());
        _joysticks[joystickId]->_defaultTextureInnerEnabled = true;
    }

    if (regionOuter)
    {
        _joysticks[joystickId]->_regionOuter.x = regionOuter->x;
        _joysticks[joystickId]->_regionOuter.y = regionOuter->y;
        _joysticks[joystickId]->_regionOuter.width = regionOuter->width;
        _joysticks[joystickId]->_regionOuter.height = regionOuter->height;
    }

    if (textureRegionOuter)
    {
        _joysticks[joystickId]->_defaultTexCoordOuter.u1 = (textureRegionOuter->x / _texture->getWidth());
        _joysticks[joystickId]->_defaultTexCoordOuter.v1 = 1.0f - (textureRegionOuter->y / _texture->getHeight());
        _joysticks[joystickId]->_defaultTexCoordOuter.u2 = _joysticks[joystickId]->_defaultTexCoordOuter.u1 + (textureRegionOuter->width / _texture->getWidth());
        _joysticks[joystickId]->_defaultTexCoordOuter.v2 = _joysticks[joystickId]->_defaultTexCoordOuter.v1 - (textureRegionOuter->height / _texture->getHeight());
        _joysticks[joystickId]->_defaultTextureOuterEnabled = true;
    }
}

bool Gamepad::isJoystickActive(unsigned int index) const
{
    assert(index < _joystickCount);

    return _joysticks[index]->_contactIndex != INVALID_CONTACT_INDEX;
}

const Vector2& Gamepad::getJoystickState(unsigned int index) const
{
    assert(index < _joystickCount);

    return _joysticks[index]->_direction;
}

void Gamepad::setSpriteBatch(SpriteBatch* spriteBatch)
{
    _spriteBatch = spriteBatch;
}

void Gamepad::draw(const Vector4& color)
{
    _spriteBatch->begin();
    
    // Draw buttons.
    for (unsigned int i = 0; i < _buttonCount; ++i)
    {
        if ((_buttons[i]->_pressed == BUTTON_PRESSED) && (_buttons[i]->_focusTextureEnabled))
        {
            _spriteBatch->draw(_buttons[i]->_region.x, _buttons[i]->_region.y,  _buttons[i]->_region.width,  _buttons[i]->_region.height,
                           _buttons[i]->_focusTexCoord.u1, _buttons[i]->_focusTexCoord.v1, _buttons[i]->_focusTexCoord.u2, _buttons[i]->_focusTexCoord.v2,
                           color);
            continue;
        }
        else if (_buttons[i]->_defaultTextureEnabled)
        {
            _spriteBatch->draw(_buttons[i]->_region.x, _buttons[i]->_region.y,  _buttons[i]->_region.width,  _buttons[i]->_region.height,
                           _buttons[i]->_defaultTexCoord.u1, _buttons[i]->_defaultTexCoord.v1, _buttons[i]->_defaultTexCoord.u2, _buttons[i]->_defaultTexCoord.v2,
                           color);
        }
    }

    // Draw joysticks.
    for (unsigned int i = 0; i < _joystickCount; ++i)
    {
        float x, y, width, height;
        float u1, v1, u2, v2;

        if (_joysticks[i]->_defaultTextureOuterEnabled)
        {
            // Draw Outer joggle.
            x = _joysticks[i]->_regionOuter.x;
            y = _joysticks[i]->_regionOuter.y;
            width = _joysticks[i]->_regionOuter.width;
            height = _joysticks[i]->_regionOuter.height;

            // Adjust outer joggle position to point to top right.
            x = x - (width * 0.5f);
            y = y - (height * 0.5f);
        
            float u1 = _joysticks[i]->_defaultTexCoordOuter.u1;
            float v1 = _joysticks[i]->_defaultTexCoordOuter.v1;
            float u2 = _joysticks[i]->_defaultTexCoordOuter.u2;
            float v2 = _joysticks[i]->_defaultTexCoordOuter.v2;

            _spriteBatch->draw(x, y, width,  height, u1, v1, u2, v2, color);
        }

        if (_joysticks[i]->_defaultTextureInnerEnabled)
        {
            // Draw Inner joggle.
            x = _joysticks[i]->_regionInner.x;
            y = _joysticks[i]->_regionInner.y;
            width = _joysticks[i]->_regionInner.width;
            height = _joysticks[i]->_regionInner.height;
            float radius = _joysticks[i]->_radius;
            float displacementX = _joysticks[i]->_displacement.x;
            float displacementY = _joysticks[i]->_displacement.y;

            // Move position to reflect displacement.
            if (((displacementX * displacementX) + (displacementY * displacementY)) <= (radius * radius))
            {
                x += displacementX;
                y += displacementY;
            }
            else
            {
                // Find the point on the joystick's circular bound where
                // the vector intersects. This would be the position of the 
                // inner joggle.
                Vector2 delta = Vector2(displacementX, displacementY);
                delta.normalize();
                delta.scale(radius);
                x += delta.x;
                y += delta.y;
            }
        
            // Adjust inner joggle position to point to top right.
            x = x - (width * 0.5f);
            y = y - (height * 0.5f);

            u1 = _joysticks[i]->_defaultTexCoordInner.u1;
            v1 = _joysticks[i]->_defaultTexCoordInner.v1;
            u2 = _joysticks[i]->_defaultTexCoordInner.u2;
            v2 = _joysticks[i]->_defaultTexCoordInner.v2;
        
            _spriteBatch->draw(x, y, width, height, u1, v1, u2, v2, color);
        }
    }

    _spriteBatch->end();
}

void Gamepad::touch(int x, int y, int touchEvent, unsigned int contactIndex)
{
    if (contactIndex >= MAX_TOUCH_INPUTS)
        return;

    for (unsigned int i = 0; i < _buttonCount; ++i)
    {
        switch (touchEvent)
        {
            case Touch::TOUCH_PRESS:
            {
                if (_buttons[i]->_contactIndex == INVALID_CONTACT_INDEX)
                {
                    if ((x >= _buttons[i]->_region.x) && (x <= (_buttons[i]->_region.x + _buttons[i]->_region.width)) &&
                                (y >= _buttons[i]->_region.y) && (y <= (_buttons[i]->_region.y + _buttons[i]->_region.height)))
                    {
                        _buttons[i]->_contactIndex = contactIndex;
                        _buttons[i]->_pressed = BUTTON_PRESSED;
                    }
                }
            }
            break;
            case Touch::TOUCH_RELEASE:
            {
                if (_buttons[i]->_contactIndex == contactIndex)
                {
                    _buttons[i]->_contactIndex = -1;
                    _buttons[i]->_pressed = BUTTON_RELEASED;
                }
            }
            break;
        }
    }

    for (unsigned int i = 0; i < _joystickCount; ++i)
    {
        switch (touchEvent)
        {
            case Touch::TOUCH_PRESS:
            {
                // Activate the joystick only when it is touched inside the
                // outer joggle.
                float bx = _joysticks[i]->_regionInner.x;
                float by = _joysticks[i]->_regionInner.y;
                float bradius = _joysticks[i]->_radius;

                if ((x >= (bx - bradius)) && (x <= (bx + bradius)) &&
                    (y >= (by - bradius)) && (y <= (by + bradius)) && _joysticks[i]->_contactIndex == INVALID_CONTACT_INDEX)
                    {
                        _joysticks[i]->_contactIndex = contactIndex;
                        _joysticks[i]->_displacement.set(0.0f, 0.0f);
                        _joysticks[i]->_direction.set(0.0f, 0.0f);
                    }
            }
            case Touch::TOUCH_MOVE:
            {
                if (_joysticks[i]->_contactIndex == contactIndex)
                {
                    float displacementX = x - _joysticks[i]->_regionInner.x;
                    float displacementY = y - _joysticks[i]->_regionInner.y; 
                    float radius = _joysticks[i]->_radius;

                    // Determine the position of the finger. If finger moves beyond the joystick radius,
                    // clamp the direction vector to bounding circle.
                    if (((displacementX * displacementX) + (displacementY * displacementY)) <= (radius * radius))
                    {
                        // Negate the y in the direction vector so that y is positive up.
                        _joysticks[i]->_direction.set(displacementX, -displacementY);
                        _joysticks[i]->_direction.scale(1.0f / radius);
                    }
                    else
                    {
                        // Negate the displacementY here so that y is positive up.
                        Vector2 delta = Vector2(displacementX, -displacementY);
                        delta.normalize();
                        delta.scale(radius);
                        delta.normalize();
                        _joysticks[i]->_direction.set(delta);
                    }

                    _joysticks[i]->_displacement.set(displacementX, displacementY);
                }
            }
            break;
            case Touch::TOUCH_RELEASE:
            {
                if (_joysticks[i]->_contactIndex == contactIndex)
                {
                    // Reset displacement and direction vectors.
                    _joysticks[i]->_contactIndex = -1;
                    _joysticks[i]->_displacement.set(0.0f, 0.0f);
                    _joysticks[i]->_direction.set(0.0f, 0.0f);
                }
            }
            break;
        }
    }
}
