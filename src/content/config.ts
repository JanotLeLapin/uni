import { z, defineCollection } from "astro:content";

const maths = defineCollection({
  type: "content",
  schema: z.object({
    title: z.string(),
    katex: z.boolean().default(true),
  }),
});
const physique = defineCollection({
  type: "content",
  schema: z.object({
    title: z.string(),
    katex: z.boolean().default(true),
  }),
});

export const collections = {
  maths,
  physique,
}
