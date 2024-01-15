import { defineConfig } from 'astro/config';

// https://astro.build/config
export default defineConfig({
  base: '/uni',
  markdown: {
    remarkPlugins: ['remark-math'],
    rehypePlugins: [['rehype-katex', {}]],
  },
});
