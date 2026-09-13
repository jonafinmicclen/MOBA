#pragma once

// Render-space Z (height) the loaded map mesh's root sits at - see
// GameArgsHandler::applyGameArgs, which places the map entity here. The
// debug walkable grid overlay (GameClient::render) intentionally draws at
// this same height, to show where the real ground is.
//
// IMPORTANT: don't "find" this value by eyeballing the grid overlay's
// height alone - it draws with depth testing off, so any height can look
// aligned on screen from one camera angle without actually matching this
// mesh's true world height. Only change this value based on where the
// visible map mesh itself actually is.
constexpr float kMapGroundHeight = 17.5f;
