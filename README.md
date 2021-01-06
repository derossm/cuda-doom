# CUDA Doom

I have forked Crispy Doom as a personal hobby project. It is my goal to take this successful, moderately sized legacy-C code base and retrofit all aspects to Modern C++20 alongside CUDA integration and with (as near as possible) full conformance to Microsoft's Core Guidelines.

I will be actively disregarding the existing style, and refraining from the sensible approach to make small changes, test, repeat approach, despite both being the reasonable and universally recommended approach when professionally maintaining and/or updating production code; instead I will systematically refine my personal style as I go, adapting to any pitfalls I encounter, while tearing the entire engine apart to both see how it works and try new things.

My goal isn't to keep the engine working at each stage, and I realize my approach would be ridiculous if my goal were to ship a product or meet any release schedule.

Currently, I am learning how things interact, and picking sections of code to tackle as they interest me. I am slowly refactoring out all incompatible C-code (such as void* malloc calls) while I think about better ways to replace short term changes with new designs.

I will try to bring in standard algorithms where possible, but also profile the original C-code with my own code whenever I get anything to compile and be mindful of any unreasonable performance loss with abstractions. I will be getting frequently lost in weeds, focusing on corner cases, over thinking minute details, and generally wasting time on code that I know will be thrown away just to see how different things work.

I'm planning to design unit tests alongside new code as I go, with C-compatible versions, and then comparing my C++ measurements against the original Crispy Doom C-code.

I don't recommend following this fork.
