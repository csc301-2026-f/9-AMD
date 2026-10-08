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

 * At least 5 user stories concerning the main features of the application - note that this can broken down further
 * You must follow proper user story format (as taught in lecture) ```As a <user of the app>, I want to <do something in the app> in order to <accomplish some goal>```
 * User stories must contain acceptance criteria. Examples of user stories with different formats can be found here: https://www.justinmind.com/blog/user-story-examples/. **It is important that you provide a link to an artifact containing your user stories**.
 * If you have a partner, these must be reviewed and accepted by them. You need to include the evidence of partner approval (e.g., screenshot from email) or at least communication to the partner (e.g., email you sent)

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


#### Q7: What are the roles & responsibilities on the team?

Describe the different roles on the team and the responsibilities associated with each role (e.g., frontend, database). 
 * Roles should reflect the structure of your team and be appropriate for your project. One person may have multiple roles.  
 * Add role(s) to your Team-[Team_Number]-[Team_Name].csv file on the main folder.
 * At least one person must be identified as the dedicated partner liaison. They need to have great organization and communication skills.
 * Everyone must contribute to code. Students who don't contribute to code enough will receive a lower mark at the end of the term.

List each team member and:
 * A description of their role(s) and responsibilities including the components they'll work on and non-software related work
 * Why did you choose them to take that role? Specify if they are interested in learning that part, experienced in it, or any other reasons. Do no make things up. This part is not graded but may be reviewed later.


#### Q8: How will you work as a team?
We plan to meet weekly as a team over discord to share progress, discuss any issues, and plan our next steps. We will also schedule additional meetings or coding sessions when needed, especially for debugging and integrating different parts of the project. We will also use GitHub issues to track tasks and pull requests to review each other's code.

We have been meeting with our AMD project partner to discuss the project scope and expectations. Our first meeting focused on exploring possible directions for extending Schola, including advanced inference, environment generation, and agent benchmarking. A follow-up meeting was planned to narrow down these ideas and agree on a realistic scope for the semester.

For the rest of the term, we plan to meet with our partner regularly over Microsoft Teams to share updates, ask questions, and get feedback on our progress. We will use Teams for day-to-day communication and email for more formal updates.

Describe meetings (and other events) you are planning to have. 
 * When and where? Recurring or ad hoc? In-person or online?
 * What's the purpose of each meeting?
 * Other events could be coding sessions, code reviews, quick weekly sync meeting online, etc.
 * You should have 2 meetings with your project partner (if you have one) before D1 is due. Describe them here:
   * You must keep track of meeting minutes and add them to your repo under "deliverables/minutes" folder
   * You must have a regular meeting schedule established for the rest of the term.  
  
#### Q9: How will you organize your team?

List/describe the artifacts you will produce to organize your team. (We strongly recommend that you use standard collaboration tools like Linear.app, Jira, Slack, Discord, GitHub.)       

 * Artifacts can be To-Do lists, Task boards, schedule(s), meeting minutes, etc.
 * We want to understand:
   * How do you keep track of what needs to get done? (You must grant your TA and partner access to systems you use to manage work)
   * **How do you prioritize tasks?**
   * How do tasks get assigned to team members?
   * How do you determine the status of work from inception to completion?

#### Q10: What are the rules regarding how your team works?

**Communications:**
 * What is the expected frequency? What methods/channels will be used? 
 * If you have a partner project, what is your process for communicating with your partner? Who is responsible?
 
**Collaboration:**
 * How are people held accountable for attending meetings, completing action items? What is your process?
 * How will you address the issue if one person doesn't contribute or is not responsive?

## Organisation Details

#### Q11. How does your team fit within the overall team organisation of the partner?
* Given the team structure of your partner, what role do you think your team will play?
* Examples include product development that includes developing new features, or quality assurance that includes developing features that test the product reliability, or software maintenance that includes fixing crucial bugs in the product.
* Provide examples of why you think you fit this role.

#### Q12. How does your project fit within the overall product from the partner?
* Look at the big picture of the product and think about how your project fits into this product.
* Is your project the first step towards building this product? Is it the first prototype? Are you developing the frontend of a product whose backend is developed by the partner? Are you building the release pipelines for a product that is developed by the partner? Are you building a core feature set and take full ownership of these features?
* You should also provide details of who else is contributing to what parts of the product, if you have this information. This is more important if the project that you will be working on has strong coupling with parts that will be contributed to by members other than your team (e.g., from a partner).
* You can be creative for these questions and even use a graphical or pictorial representation to demonstrate the fit.
* Briefly specify what your partner considers a success for this project. Do they want you to build specific features? Publish a usable product? Just a prototype? Be as specific as you can be at this point.

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

#### Q14. What are some potential mitigation strategies for the risks you identified?
* Examples of mitigation strategies:
  * More communication with the partner might help with improving clarity.
  * Adding more details for an user story might make it less abstract.
  * Adding an extra user story might increase the project complexity, making it less simple.
* It's ok if you are unable to find mitigation strategies for all the risks right now.
