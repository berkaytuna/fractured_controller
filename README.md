Gameplay Video: https://youtu.be/JRA4mCSgwn8

** Tested with Display Resolution 1920x1080, 100% Screen Zoom. Changing these will cause different program behaviour than expected, but you can adjust settings file to change how this program behaves.

Limitations:
- As this program simulates mouse controls, in small areas with interactable objects
movement could feel clunky. When stucked, try to move away from interactable objects.
- Probably not suitable for any competitive content, mainly due to how skill activation
and target selection works in this game, which are heavily dependant on precise mouse controls.

Introduction:
- This program enables an unofficial controller support for the game Fractured Online using XInput. The controller should be compatible with XInput. Steam Input should be disabled for the game.

Features:
- Movement with left stick
- Mouse controls with right stick
- In total 20 assignable keys
- 3 different key press type depending on skill type
- Hide cursor when not needed (Autohotkey v1 has to be installed)

Keys:
- 4 Dpad Keys
- L3, R3, Back, Start
- (action keys) 12 Keys consisting two sets of L1, R1, a, b, x, y

Function:
- L2 is activator.
- If L2 not pressed, R2 behaves as left mouse click and first set of action keys are active.
- If L2 is pressed, R2 behaves as right mouse click and second set of action keys are active.

Key Press Type:
- There are 3 key press types: Normal, Target and AoE.
This is a very basic interpretation of game's ability system.
- Normal (0): Sends key code directly, for example M key to open map. Intended for UI interaction and skills that do not require a target, for example fireball. Fires the skill in the direction character is moving.
- Target (1): Sends key code with repetitions and extends the mouse every repetition in the direction player moving. Intended for target based skills. You have to move directly to a target, for skill to sucessfully fire.
- AoE (2): Moves the mouse to the far position from character in the moving direction of character and fires the skill. For example useful for teleportation.

Settings:
- All codes are decimal virtual key codes that represent key presses. For example 87 for W.
- ahk_path: Full path to Autohotkey installation.
- [button]: x for set 1 key (L2 not pressed), x_2 for set 2 (L2 pressed).
- [type]: 0, 1, 2 for above mentioned key press types.
