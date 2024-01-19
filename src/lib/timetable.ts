export type TTDescription = {
  type: string;
  location: string;
  subject: string;
};

export type TTEntry = {
  id: string;
  start: Date;
  end: Date;
  allDay: boolean;
  description: TTDescription;
  backgroundColor: string;
  textColor: string;
  eventCategory: string;
  sites: string[];
}

const decodeHTML = (raw: string) => raw.replace(/&#(\d+);/g, (_, dec) => String.fromCharCode(dec));
export const parseDescription = (description: string) => {
  const [type, location, subject] = description.split("\r\n\r\n<br />\r\n\r\n").map(decodeHTML);
  return { type, location, subject };
}

export const fetchTimetable = async (id: string, date: Date): Promise<TTEntry[]> => {
  const d = new Date(date.getFullYear(), date.getMonth(), date.getDate() - date.getDay());

  const year = date.getFullYear();
  const month = date.getMonth() + 1;
  const url = `https://corsproxy.io/?${encodeURIComponent("https://edt.uvsq.fr/Home/GetCalendarData")}`;
  const body = `start=${year}-${month}-${d.getDate()}&end=${year}-${month}-${d.getDate() + 5}&resType=103&calview=month&federationIds%5B%5D=${id}&colourScheme=3`;
  const json = await fetch(url, {
    method: "POST",
    headers: { "Content-Type": "application/x-www-form-urlencoded; charset=UTF-8", },
    body
  }).then(res => res.json())

  return json
    .map((tt: any) => ({
      ...tt,
      description: parseDescription(tt.description),
      start: new Date(tt.start),
      end: new Date(tt.end),
    })) as TTEntry[];
}
