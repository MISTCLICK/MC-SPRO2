import fs from "node:fs";
import { serve } from "@hono/node-server";
import { Hono } from "hono";

type DataRecord = {
  angle: number;
  speed: number;
  time: number;
};

const globalRecords: DataRecord[] = [];
const app = new Hono();

app.get("/", async (c) => {
  await fs.promises.writeFile(`./fs.output.csv`, "time,angle,speed\n");

  return c.json({ success: true, query: c.req.queries() });
});

app.get("/data/bus", async (c) => {
  const query = c.req.query() as unknown as DataRecord;

  globalRecords.push(query);
  console.log(globalRecords);

  await fs.promises.appendFile(
    "./fs.output.csv",
    `${query.time},${query.angle},${query.speed}\n`
  );

  return c.json({
    success: true,
  });
});

app.get("/data/read", async (c) => {
  return c.json({
    success: true,
    data: globalRecords,
  });
});

serve(
  {
    fetch: app.fetch,
    port: 3000,
  },
  (info) => {
    console.log(`Server is running on http://localhost:${info.port}`);
  }
);
