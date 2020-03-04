# Basis Texture Plugin
This adds support for loading and saving .basis texture files:

https://github.com/BinomialLLC/basis_universal

One edit is required in the basis lib. In basisu_comp.h make the compressor::m_basis_file member public. You can get the edited version here:
https://github.com/Leadwerks/basis_universal
