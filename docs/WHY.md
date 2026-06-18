# Why

OpenGL and Vulkan are good tools. They are large because real hardware is large.

DEADGL is smaller on purpose. It draws to a framebuffer, hashes the result, and writes PPM. A line is integer steps. A triangle is edge tests. A file is bytes.

This is not nostalgia. It is control.
