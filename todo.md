# Current Design Notes (Redesign / Refactor / Foundation Development)

- "Advancement Status" rich object fed into display adaptors
- What is an advancement?
    - Key: category/shortname (FQN)
    - Basic data: Icon (texture ptr?) / Short Name (str) / Category (str)
    - Criterion data: Missing / Completed? Icons optional?

# Todo for AASSG

1. Basic animation on turntables
    - Render background sprites
2. AdvancementsFileProvider <- file stuff only done here
3. AdvancementsProvider

## Features

- F3+C + Alt1... -> initial keypress opens "current coords, press [keybind] for... xyz" in tracker? configuration string for coords/nether coords?
- Checklist based tiered/configurable helper/tutorial system
- ALERT: Biome X missing [you have entered: BIOME y]
- ALERT: Food X missing
