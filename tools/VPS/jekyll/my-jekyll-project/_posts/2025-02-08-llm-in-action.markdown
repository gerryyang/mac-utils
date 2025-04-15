---
layout: post
title:  "LLM in Action"
date:   2025-02-08 12:00:00 +0800
categories: ML
---

* Do not remove this line (it will not be displayed)
{:toc}


# [Intro to Large Language Models - Andrej Karpathy](https://www.youtube.com/watch?v=zjkBMFhNj_g)

This is a 1 hour general-audience introduction to Large Language Models: the core technical component behind systems like ChatGPT, Claude, and Bard. What they are, where they are headed, comparisons and analogies to present-day operating systems, and some of the security-related challenges of this new computing paradigm.

As of November 2023 (this field moves fast!).

Context: This video is based on the slides of a talk I gave recently at the AI Security Summit. The talk was not recorded but a lot of people came to me after and told me they liked it. Seeing as I had already put in one long weekend of work to make the slides, I decided to just tune them a bit, record this round 2 of the talk and upload it here on YouTube. Pardon the random background, that's my hotel room during the thanksgiving break.

Slides as PDF: https://drive.google.com/file/d/1pxx_... (42MB)
Slides. as Keynote: https://drive.google.com/file/d/1FPUp... (140MB)

Few things I wish I said (I'll add items here as they come up):

* The dreams and hallucinations do not get fixed with finetuning. Finetuning just "directs" the dreams into "helpful assistant dreams". Always be careful with what LLMs tell you, especially if they are telling you something from memory alone. That said, similar to a human, if the LLM used browsing or retrieval and the answer made its way into the "working memory" of its context window, you can trust the LLM a bit more to process that information into the final answer. But TLDR right now, do not trust what LLMs say or do. For example, in the tools section, I'd always recommend double-checking the math/code the LLM did.

* How does the LLM use a tool like the browser? It emits special words, e.g. `|BROWSER|`. When the code "above" that is inferencing the LLM detects these words it captures the output that follows, sends it off to a tool, comes back with the result and continues the generation. How does the LLM know to emit these special words? Finetuning datasets teach it how and when to browse, by example. And/or the instructions for tool use can also be automatically placed in the context window (in the “system message”).
* You might also enjoy my 2015 blog post "Unreasonable Effectiveness of Recurrent Neural Networks". The way we obtain base models today is pretty much identical on a high level, except the RNN is swapped for a Transformer. http://karpathy.github.io/2015/05/21/...
* What is in the run.c file? A bit more full-featured 1000-line version hre: https://github.com/karpathy/llama2.c/...

```
Chapters:

Part 1: LLMs
00:00:00 Intro: Large Language Model (LLM) talk
00:00:20 LLM Inference
00:04:17 LLM Training
00:08:58 LLM dreams
00:11:22 How do they work?
00:14:14 Finetuning into an Assistant
00:17:52 Summary so far
00:21:05 Appendix: Comparisons, Labeling docs, RLHF, Synthetic data, Leaderboard

Part 2: Future of LLMs
00:25:43 LLM Scaling Laws
00:27:43 Tool Use (Browser, Calculator, Interpreter, DALL-E)
00:33:32 Multimodality (Vision, Audio)
00:35:00 Thinking, System 1/2
00:38:02 Self-improvement, LLM AlphaGo
00:40:45 LLM Customization, GPTs store
00:42:15 LLM OS

Part 3: LLM Security
00:45:43 LLM Security Intro
00:46:14 Jailbreaks
00:51:30 Prompt Injection
00:56:23 Data poisoning
00:58:37 LLM Security conclusions

End
00:59:23 Outro
```

# [Deep Dive into LLMs like ChatGPT - Andrej Karpathy](https://www.youtube.com/watch?v=7xTGNNLPyMI)

This is a general audience deep dive into the Large Language Model (LLM) AI technology that powers ChatGPT and related products. It is covers the full training stack of how the models are developed, along with mental models of how to think about their "psychology", and how to get the best use them in practical applications. I have one "Intro to LLMs" video already from ~year ago, but that is just a re-recording of a random talk, so I wanted to loop around and do a lot more comprehensive version.

Instructor

Andrej was a founding member at OpenAI (2015) and then Sr. Director of AI at Tesla (2017-2022), and is now a founder at Eureka Labs, which is building an AI-native school. His goal in this video is to raise knowledge and understanding of the state of the art in AI, and empower people to effectively use the latest and greatest in their work.
Find more at https://karpathy.ai/ and https://x.com/karpathy

```
Chapters

00:00:00 introduction
00:01:00 pretraining data (internet)
00:07:47 tokenization
00:14:27 neural network I/O
00:20:11 neural network internals
00:26:01 inference
00:31:09 GPT-2: training and inference
00:42:52 Llama 3.1 base model inference
00:59:23 pretraining to post-training
01:01:06 post-training data (conversations)
01:20:32 hallucinations, tool use, knowledge/working memory
01:41:46 knowledge of self
01:46:56 models need tokens to think
02:01:11 tokenization revisited: models struggle with spelling
02:04:53 jagged intelligence
02:07:28 supervised finetuning to reinforcement learning
02:14:42 reinforcement learning
02:27:47 DeepSeek-R1
02:42:07 AlphaGo
02:48:26 reinforcement learning from human feedback (RLHF)
03:09:39 preview of things to come
03:15:15 keeping track of LLMs
03:18:34 where to find LLMs
03:21:46 grand summary
```

Links

* ChatGPT https://chatgpt.com/
* FineWeb (pretraining dataset): https://huggingface.co/spaces/Hugging...
* Tiktokenizer: https://tiktokenizer.vercel.app/
* Transformer Neural Net 3D visualizer: https://bbycroft.net/llm
* llm.c Let's Reproduce GPT-2 https://github.com/karpathy/llm.c/dis...
* Llama 3 paper from Meta: https://arxiv.org/abs/2407.21783
* Hyperbolic, for inference of base model: https://app.hyperbolic.xyz/
* InstructGPT paper on SFT: https://arxiv.org/abs/2203.02155
* HuggingFace inference playground: https://huggingface.co/spaces/hugging...
* DeepSeek-R1 paper: https://arxiv.org/abs/2501.12948
* TogetherAI Playground for open model inference: https://api.together.xyz/playground
* AlphaGo paper (PDF): https://discovery.ucl.ac.uk/id/eprint...
* AlphaGo Move 37 video:    • Lee Sedol vs AlphaGo  Move 37 reactio...
* LM Arena for model rankings: https://lmarena.ai/
* AI News Newsletter: https://buttondown.com/ainews
* LMStudio for local inference https://lmstudio.ai/
* The visualization UI I was using in the video: https://excalidraw.com/
* The specific file of Excalidraw we built up: https://drive.google.com/file/d/1EZh5...
* Discord channel for Eureka Labs and this video:   / discord





# Papers

* https://papers.cool
* https://www.aminer.cn/
* https://consensus.app/

再通过元宝上传 pdf 总结精读。








