# YOUR PRODUCT/TEAM NAME
> _Note:_ This document will evolve throughout your project. You commit regularly to this file while working on the project (especially edits/additions/deletions to the _Highlights_ section). 
 > **This document will serve as a master plan between your team, your partner and your TA.**

## Product Details
 
#### Q1: What is the product?

An extension to AMD Schola, an Unreal Engine library for training and running reinforcement learning (RL) agents, to build smarter and more optimized NPCs in games.

Schola lets developers train RL agents in Unreal Engine and use them in games. Today it assumes each agent runs inference every frame, one at a time. This is an undesirable pattern in many scenarios:
- In turn based games, per-frame inference wastes compute.
- A simulations with thousands of pedestrains can't afford a separate inference call per character (building on top of Unreal MassEntity).

We are building an extension to Schola, delivered as part of an Unreal Engine plugin with Python-side support, that adds optimization and support for five NPC types:
1. **Adaptive NPCs** learn online from player behavior and adjust during play.
2. **Event-Based NPCs** run inference only when subscribed game events fire, such as a turn starting or a player entering a zone.
3. **Background NPCs** make large crowds affordable through batched inference, culling of off-screen agents, and integration with Unreal's Mass Entity framework.
4. **Shared-Policy NPCs** reuse a common network base with specialized outputs for different roles, so related behaviors run faster than separate models.
5. **Advanced Strategy NPCs** *(optional stretch goal)* look ahead by simulating opponent moves, as top strategy-game AIs do. This is hard to do with Schola's current inference interface.

Our project lets adapt and deploy our NPC implementations in their games, instead of writing training and inference boilerplate. We target game developers using Unreal Engine who want learned NPC behavior across genres, especially turn-based strategy, FPS, and large-scale simulations, without building custom infrastructure.

Alexander Cann, Member of Technical Staff at AMD, is our partner and lead developer of Schola. His interests cover deep neural networks, reinforcement learning, and the intersection of AI and games.

Success looks like when a developer can seamlessly choose the correct NPC type, attach to a trained model, and get efficient behavior for their game genre and usecase.

#### Q2: Who are your target users?

Primary user: A gameplay or AI programmer at an indie or mid-sized studio. They work in Unreal Engine daily using C++ and Blueprints, and know behavior trees and navigation well. They have basic machine learning knowledge, such as having followed a tutorial or trained a simple model, but they are not ML researchers.

What they want: NPCs that feel smarter than hand-scripted behavior trees, without building a custom ML pipeline. For example, a programmer on a turn-based strategy game who wants an AI opponent that learns good tactics, but can't spend weeks on infrastructure.

Their pain points: Running RL models in a shipping game means writing custom code for batching and scheduling. Frame budgets are tight, so per-frame inference for every NPC is too costly.

Secondary users:
- Technical or AI designers who want an streamlined UI/API to tune advanced NPC behavior in Unreal.
- Machine learning engineers on game teams who train policies in Python (PyTorch, Stable Baselines3, Ray RLlib) and need an easy path to deploy them in Unreal.
- Solo developers and game jam teams experimenting with learned NPCs, who benefit most from ready-made examples.

#### Q3: Why would your users choose your product? What are they using today to solve their problem/need?

Most teams today write custom inference code for each NPC type, run every agent's model every frame, or stick with behavior trees. Schola covers training and basic inference, but turn-based triggers, crowds, shared networks and online adaptation are left to the developer. Our project:

- Saves time and money, helping developers avoid implementing writing boilerplate
- Provides optimized NPC implementations, increasing performance and lowering demands on hardware
- Makes advanced ML/RL techniques more accessible, such as through pretrained weights for common cases (e.g. basic crowd movement)
- Enables completely new behavior, such as online learning powered NPCs

Existing alternatives: Unreal's behavior trees and Mass Entity cover scripted behavior and large-scale simulation, not learned policies. Generic ML runtimes run models but do not typically offer support for turns, culling or entity batches.

Partner fit: GPUOpen is AMD's open-source initiative giving game developers free tools to get the most out of GPU hardware. Efficient, batched, open-source inference for NPCs is squarely in that mission.

#### Q4: What are the user stories that make up the Minumum Viable Product (MVP)?

* Event-based NPCs: As a game developer, I want NPCs to subscribe to selected gameplay events and perform inference when those events occur, so that I can coordinate their decisions with turn changes or other relevant game events.
* Learning during gameplay: As a game developer, I want NPC policies to learn from player interactions during gameplay, so that NPCs can adapt their strategies to the player’s behavior.
* Efficient background agents: As a researcher, I want to batch inference requests across many background agents, including agents managed through Unreal’s Mass Entity framework, so that I can study large-scale interactions between ML-controlled agents while staying within the simulation’s performance budget.
* Shared-policy NPCs: As a machine learning engineer, I want NPC policies with a shared network backbone and specialized output heads to reuse the backbone’s computation for the same observation, so that I can produce multiple specialized decisions with less computation than running each policy independently.
* Advanced strategy NPCs: As a game developer, I want NPCs to evaluate possible future actions and opponent responses using a search strategy such as Monte Carlo Tree Search, so that they can make decisions that account for longer-term consequences.


#### Q5: Have you decided on how you will build it? Share what you know now or tell us the options you are considering.

We extend Schola's existing architecture. Schola already pairs a Python training side with an Unreal C++ runtime, connected by gRPC. We add new runtime modules on the Unreal side and small additions on the Python side.

(Existing) Stack:
- C++ and Blueprints for Unreal Engine 5 plugin modules
- Python for training and model export, using Schola's existing Stable Baselines3, Ray RLlib and Gymnasium adapters, plus PyTorch
- Protocol Buffers and gRPC for Python-to-Unreal communication (already in Schola)
- Unreal's Neural Network Engine (NNE), via Schola's ScholaNNE module, for in-game inference

Architecture: We simply add new modules on top of the inference and interactor layers, roughly one per NPC type:
- Event-based scheduler: triggers inference from subscribed game events
- Batched inference manager: groups many agents into one call, with culling and Mass Entity integration
- Shared-policy runner: runs a common network base once, then branch out
- Online adaptation: streams player-behavior data over gRPC to Python, which updates the policy and returns new weights
- Search-based strategy support (optional): a lookahead interface for simulating opponent moves

----
## Intellectual Property Confidentiality Agreement 
> Note this section is **not marked** but must be completed briefly if you have a partner. If you have any questions, please ask on Piazza.
>  
**By default, you own any work that you do as part of your coursework.** However, some partners may want you to keep the project confidential after the course is complete. As part of your first deliverable, you should discuss and agree upon an option with your partner. Examples include:
1. You can share the software and the code freely with anyone with or without a license, regardless of domain, for any use.
2. You can upload the code to GitHub or other similar publicly available domains.
3. You will only share the code under an open-source license with the partner but agree to not distribute it in any way to any other entity or individual. 
4. You will share the code under an open-source license and distribute it as you wish but only the partner can access the system deployed during the course.
5. You will only reference the work you did in your resume, interviews, etc. You agree to not share the code or software in any capacity with anyone unless your partner has agreed to it.

**Your partner cannot ask you to sign any legal agreements or documents pertaining to non-disclosure, confidentiality, IP ownership, etc.**

Briefly describe which option you have agreed to.

----

## Teamwork Details

#### Q6: Have you met with your team?

Do a team-building activity in-person or online. This can be playing an online game, meeting for bubble tea, lunch, or any other activity you all enjoy.
* Get to know each other on a more personal level.
* Provide a few sentences on what you did and share a picture or other evidence of your team building activity.
* Share at least three fun facts from members of you team (total not 3 for each member).

For our team-building activity, we decided to play Wordle together online. During this activity, we were able to get to know each other better and it was a lot of fun. Some fun facts we collected were that Chloe used to have a pet turtle, and both Olivia and Jace can type 160+ wpm.

<img width="455" height="345" alt="Screenshot 2026-10-08 at 12 46 17 AM" src="https://github.com/user-attachments/assets/7df4454a-9ee6-4445-b7cf-16124ce3c45a" />

#### Q7: What are the roles & responsibilities on the team?

Describe the different roles on the team and the responsibilities associated with each role (e.g., frontend, database). 
 * Roles should reflect the structure of your team and be appropriate for your project. One person may have multiple roles.  
 * Add role(s) to your Team-[Team_Number]-[Team_Name].csv file on the main folder.
 * At least one person must be identified as the dedicated partner liaison. They need to have great organization and communication skills.
 * Everyone must contribute to code. Students who don't contribute to code enough will receive a lower mark at the end of the term.

Our team is divided into four development roles, each responsible for a different class of NPC behavior. Each role is explained more in detail in **Q1.**

1. **Adaptive NPCs:** 
2. **Event-Based NPCs:** 
3. **Background NPCs:** 
4. **Shared-Policy NPCs:** 


List each team member and:
 * A description of their role(s) and responsibilities including the components they'll work on and non-software related work
 * Why did you choose them to take that role? Specify if they are interested in learning that part, experienced in it, or any other reasons. Do no make things up. This part is not graded but may be reviewed later.

Max - Partner liaison, Background NPCs

Prithvi - Event-Based and Background NPCs.

Clementine - Shared Policy NPCs: She's interested in shared-policy inference because it combines machine learning, inference optimization, and system integration. 

Jason - Online learning: He is interested in evaluating ML algorithms, as well as learning more about model updating and data pipelines

Jaela - Background/shared policy Npcs (inference): She is interested in machine learning engineering and wants to learn more about efficient model inference

Jace - Online learning: Similarly to Olivia, he is interested in this because game dev has always been a direction he’s wanted to explore

Olivia - Online learning: She is interested in this because of her strong gamedev background: online learning has a more tangible effect on games which is something she would like to explore

Chloe - Online learning: Chloe is most interested in the online learning portion because she has an interest in video games and game development, which the online learning portion is has large implications for


#### Q8: How will you work as a team?

Describe meetings (and other events) you are planning to have. 
 * When and where? Recurring or ad hoc? In-person or online?
 * What's the purpose of each meeting?
 * Other events could be coding sessions, code reviews, quick weekly sync meeting online, etc.
 * You should have 2 meetings with your project partner (if you have one) before D1 is due. Describe them here:
   * You must keep track of meeting minutes and add them to your repo under "deliverables/minutes" folder
   * You must have a regular meeting schedule established for the rest of the term.

We plan to meet weekly as a team over discord to share progress, discuss any issues, and plan our next steps. We will also schedule additional meetings or coding sessions when needed, especially for debugging and integrating different parts of the project. We will also use GitHub issues to track tasks and pull requests to review each other's code.

We have been meeting with our AMD project partner to discuss the project scope and expectations. Our first meeting focused on exploring possible directions for extending Schola, including advanced inference, environment generation, and agent benchmarking. A follow-up meeting was planned to narrow down these ideas and agree on a realistic scope for the semester.

For the rest of the term, we plan to meet with our partner regularly over Microsoft Teams to share updates, ask questions, and get feedback on our progress. We will use Teams for day-to-day communication and email for more formal updates.

#### Q9: How will you organize your team?

List/describe the artifacts you will produce to organize your team. (We strongly recommend that you use standard collaboration tools like Linear.app, Jira, Slack, Discord, GitHub.)       

 * Artifacts can be To-Do lists, Task boards, schedule(s), meeting minutes, etc.
 * We want to understand:
   * How do you keep track of what needs to get done? (You must grant your TA and partner access to systems you use to manage work)
   * **How do you prioritize tasks?**
   * How do tasks get assigned to team members?
   * How do you determine the status of work from inception to completion?

We use Discord to communicate, with separate channels for things like meeting summaries, announcements, important documents and general discussions. We will also set up a shared Google folder. For to-dos, assigning tasks and prioritizing tasks, and to-dos, we will use the GitHub Projects feature to keep track of issues, tasks, blockers and ideas. We will also use this to track the flow of a task from inception to completion.

#### Q10: What are the rules regarding how your team works?

**Communications:**

What is the expected frequency? What methods/channels will be used?

Ping each other on discord for quick internal communication, discord calls for team meetings; Microsoft teams for partner communication/ weekly meetings with AMD members/ more formal project discussions

If you have a partner project, what is your process for communicating with your partner? Who is responsible?
Through Microsoft Teams. The partner liaison (Max) is responsible for following up and communicating on behalf of the team. We take meeting notes as well and formalize ideas and expectations regularly after meetings.

**Collaboration:**

How are people held accountable for attending meetings, completing action items? What is your process?

If a group member is late/not showing up for a meeting, we will @ping them on Discord (our main communication platform). In our server, we organize by channels, such as announcements and to-do’s, and we ping members according to the message content. 

How will you address the issue if one person doesn't contribute or is not responsive?

First of all, we will try to resolve the situation through contacting the person and sorting out the reason behind their lack of contribution. We will reiterate the tasks they were assigned and the communication standards we agreed upon. However, if they are not responsive for a period of time, we will escalate the issue to the TA, outlining the efforts we made to reconcile the issue ourselves.

## Organisation Details

#### Q11. How does your team fit within the overall team organisation of the partner?
* Given the team structure of your partner, what role do you think your team will play?
* Examples include product development that includes developing new features, or quality assurance that includes developing features that test the product reliability, or software maintenance that includes fixing crucial bugs in the product.
* Provide examples of why you think you fit this role.

Our team will be taking on a product development role as we are developing new features for AMD’s existing Schola framework. We have had discussions with our partner and have decided this is a good scope for the project and AMD’s needs. We will be working on infrastructure for several NPC cases, including as mentioned as prior: adaptive, event-driven, background, shared-policy, and advanced strategy NPCs [described in more detail in **Q1**]. We fit this role because the project requires a combination of machine-learning, game-engine development and software and system architecture. All members of the team have experience with C++, RL, and building software systems through coursework, projects and internships. Z

#### Q12. How does your project fit within the overall product from the partner?
* Look at the big picture of the product and think about how your project fits into this product.
* Is your project the first step towards building this product? Is it the first prototype? Are you developing the frontend of a product whose backend is developed by the partner? Are you building the release pipelines for a product that is developed by the partner? Are you building a core feature set and take full ownership of these features?
* You should also provide details of who else is contributing to what parts of the product, if you have this information. This is more important if the project that you will be working on has strong coupling with parts that will be contributed to by members other than your team (e.g., from a partner).
* You can be creative for these questions and even use a graphical or pictorial representation to demonstrate the fit.
* Briefly specify what your partner considers a success for this project. Do they want you to build specific features? Publish a usable product? Just a prototype? Be as specific as you can be at this point.

Schola is AMD’s open-source reinforcement learning framework that connects game engines with Python-based training and inference tools. Our team is responsible for building on the existing framework by adding support for more advanced NPC behaviours and inference patterns. Specifically, we will focus on four main features: adaptive NPCs that learn from player behavior, event-based NPCs that perform inference when specific events occur, background NPCs that use batched inference for better efficiency, and shared-policy NPCs that reuse parts of neural networks. Two other student teams are working on Unity Godot ports of Schola.

A successful project would deliver working, tested, and documented implementations of the 4 core NPC features that can be integrated into Schola.

## Potential Risks

#### Q13. What are some potential risks to your project?
* Now that you have defined your project, what risks can you identify that might impact it?
* Some examples of risks at this planning stage could include:
  * Uncertainties regarding a specific feature
  * Misaligned expectations or conflicts
  * Lack of clarity in execution or decision-making
  * Limited access to data, systems, or other dependencies
  * User stories that are too abstract or too simple
* For each risk, provide a brief bullet point and then explain the risk in detail.

This project involves several NPC features, each with different requirements. Implementing and testing all four features might be challenging.

Since we are not creating a new project and simply adding onto an existing framework, we will not have as much control over the scope/implementation. 

Online learning is also a topic that is difficult and theory-heavy, so there may be a learning curve. 

We plan to have several types of steppers, each with a small scope. We may be able to comfortably finish these tasks, so we can add more types of steppers if needed. 


#### Q14. What are some potential mitigation strategies for the risks you identified?
* Examples of mitigation strategies:
  * More communication with the partner might help with improving clarity.
  * Adding more details for an user story might make it less abstract.
  * Adding an extra user story might increase the project complexity, making it less simple.
* It's ok if you are unable to find mitigation strategies for all the risks right now.

One issue noted was the scope, with multiple features with unique requirements. We could potentially lower this risk by focusing on the planning portion, creating tasks, estimating time per task and reflecting on prediction accuracy every sprint. If we notice that there are too many tasks for the time remaining, we will communicate quickly with each other and the company to determine next steps for prioritization. We may also add/remove user stories depending on how we are progressing with exploring/implementing new topics.

