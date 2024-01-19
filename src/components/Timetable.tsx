import { fetchTimetable, parseDescription, type TTEntry } from "@lib/timetable";
import { useEffect, useState } from "preact/hooks"

export default () => {
  const [timetable, setTimetable] = useState<TTEntry[]>([]);
  fetchTimetable("S2MITD-01", new Date()).then(setTimetable);

  return (<></>)
}
