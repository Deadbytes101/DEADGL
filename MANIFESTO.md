# DEADGL Manifesto

A renderer should not need a cathedral of objects before it can put a pixel down.

DEADGL has one contract:

```text
command -> memory -> image -> hash
```

No hidden frame graph. No imported model worship. No driver story. No fake engine layer.

The framebuffer is owned by the program. The scene is plain text. The output is plain bytes. The proof is plain text.

Small does not mean weak. Small means the whole machine fits in the head.

## Law

1. A command mutates the framebuffer now.
2. Unknown commands are errors.
3. Render output must be reproducible.
4. Docs must say what the code does, not decorate it.
5. If a feature hides the pipe, it waits.
