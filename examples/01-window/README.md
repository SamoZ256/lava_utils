## Window tutorial

Creating a window in Lava Engine is quite straightforward. We just need to `#include <Window/Window.hpp>` and `#include <Application/Application.hpp>` as well.

### Creating window and application instance

The window constructor takes in the arguments `uint16_t width, uint16_theight, const char* title`. Application takes in a reference to a window.

```
lv::Window window(1920, 1080, "01-window");

lv::Application application(window);
```

And we can also set the clear color of the window to whatever we want.

```
lv::g_swapChain->framebuffer.clearValues[0].color = {0.2f, 0.4f, 1.0f, 1.0f};
```

Now it is time for the main loop. Every frame, we can check if the window is open by calling `window.isOpen()`. At the beginning of each frame, we need to call `window.pollEvents()` in order to capture all the events. If you would run the application now, you would get a blank window, but with black ackground. That's because we didn't present anything to the window surface using swap chain.

So first we need to get the image we are going to write to. That's as simple as calling `lv::g_swapChain->acquireNextImage()`. We also need to bind and unbind swap chain's framebuffer.

```
lv::g_swapChain->framebuffer.bind();

...

lv::g_swapChain->framebuffer.unbind();
```

All the rendering commands will go between these two function calls (if not using custom framebuffers, we will cover that later). And now finally, we can now present the image to the window.

```
lv::g_swapChain->renderAndPresent();
```

If you woulld run your application now, you should get a window with a cyan background. Congratulations! You can now create your very own window.

Also we should now destroy all the objects we allocated.

```
application.destroy();
window.destroy();
```

That's all for the first tutorial.