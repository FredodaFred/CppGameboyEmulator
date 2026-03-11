<script setup lang="ts">
import { Command } from '@tauri-apps/plugin-shell';
import { open } from '@tauri-apps/plugin-dialog';
import { ref } from 'vue';
import {invoke} from "@tauri-apps/api/core";

const romPath = ref('');
const romSelected = ref(false);

async function launchGame() {
    try {
      await invoke('launch_emulator', { romPath: romPath.value });
      console.log('Emulator launched!');
    } catch (error) {
      console.error('Failed to launch:', error);
    }
}

async function selectFile() {
  const selected = await open({
    multiple: false,
    filters: [{
      name: 'GameBoy ROM',
      extensions: ['gb', 'gbc']
    }]
  });
  if (selected == null) {
    throw new Error("Rom not selected");
  }

  romPath.value = selected;
  romSelected.value = true;
}
</script>

<template>
  <main class="container">
    <h1>GBEmu Launcher</h1>

    <div class="selection-card">
      <button @click="selectFile">Choose ROM</button>
      <p v-if="romSelected">Selected: {{ romPath }}</p>
      <p v-else>No ROM selected</p>
    </div>

    <button :disabled="!romSelected" @click="launchGame" class="play-btn">
      LAUNCH EMULATOR
    </button>
  </main>
</template>

<style scoped>

play-btn {
  width: 50px;
}

.logo.vite:hover {
  filter: drop-shadow(0 0 2em #747bff);
}

.logo.vue:hover {
  filter: drop-shadow(0 0 2em #249b73);
}

</style>

<style>
:root {
  font-family: Inter, Avenir, Helvetica, Arial, sans-serif;
  font-size: 16px;
  line-height: 24px;
  font-weight: 400;

  color: #0f0f0f;
  background-color: #f6f6f6;

  font-synthesis: none;
  text-rendering: optimizeLegibility;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  -webkit-text-size-adjust: 100%;
}

.container {
  margin: 0;
  padding-top: 10vh;
  display: flex;
  flex-direction: column;
  justify-content: center;
  text-align: center;
}

.logo {
  height: 6em;
  padding: 1.5em;
  will-change: filter;
  transition: 0.75s;
}

.logo.tauri:hover {
  filter: drop-shadow(0 0 2em #24c8db);
}

.row {
  display: flex;
  justify-content: center;
}

a {
  font-weight: 500;
  color: #646cff;
  text-decoration: inherit;
}

a:hover {
  color: #535bf2;
}

h1 {
  text-align: center;
}

input,
button {
  border-radius: 8px;
  border: 1px solid transparent;
  padding: 0.6em 1.2em;
  font-size: 1em;
  font-weight: 500;
  font-family: inherit;
  color: #0f0f0f;
  background-color: #ffffff;
  transition: border-color 0.25s;
  box-shadow: 0 2px 2px rgba(0, 0, 0, 0.2);
}

button {
  cursor: pointer;
}

button:hover {
  border-color: #396cd8;
}
button:active {
  border-color: #396cd8;
  background-color: #e8e8e8;
}

input,
button {
  outline: none;
}

#greet-input {
  margin-right: 5px;
}

@media (prefers-color-scheme: dark) {
  :root {
    color: #f6f6f6;
    background-color: #2f2f2f;
  }

  a:hover {
    color: #24c8db;
  }

  input,
  button {
    color: #ffffff;
    background-color: #0f0f0f98;
  }
  button:active {
    background-color: #0f0f0f69;
  }
}

</style>