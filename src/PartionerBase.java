import com.google.common.base.Joiner;
import com.google.common.collect.ImmutableList;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.List;

public class PartionerBase implements Partioner {
  @Override
  public Integer partition(List<Integer> array, Integer idxPivot) {
    // Пока зависит от выбора опоры
    Integer size = array.size();
    Integer pivot = array.get(idxPivot);
    Integer i = 0;

    //@Pre
    if (!pivot.equals(array.get(idxPivot)))
      throw new RuntimeException("Befor cycle partioning.");

    // Проходим все, но проскакиваем указатель на опору
    for (Integer j = 0; j < size; j++) {
      if (j.equals(idxPivot)) continue;
      if (pivot > array.get(j)) {
        if (i.equals(idxPivot)) i++;
        if (i.equals(idxPivot) || j.equals(idxPivot))
          throw new RuntimeException("Active move pivot");
        swap(array, j, i);
        i++;
      }
    }

    //@Post
    if (!pivot.equals(array.get(idxPivot)))
      throw new RuntimeException("Pivot moved!");

    // Граница будет указывать на первый правый
    if (inLeftPart(idxPivot, i)) i--;

    // Меняем индексы
    swap(array, idxPivot, i);

    // Похоже ошибка, когда выбранный стержень в левой часте
    // p in left and pivot in right
    if (pivot != array.get(i))
      throw new RuntimeException("Pivot not in right place");

    return i;
  }

  private boolean inLeftPart(Integer idxPivot, Integer i) {
    if (idxPivot < i) return true;
    return false;
  }

  // http://www.javaworld.com/javaworld/javaqa/2000-05/03-qa-0526-pass.html
  private void swap(List<Integer> array, Integer a, Integer b) {
    Integer tmp = array.get(a);
    array.set(a, array.get(b));
    array.set(b, tmp);
  }
}
