.class Lcom/racas/black/CinemaActivity$12;
.super Ljava/lang/Object;
.source "CinemaActivity.java"

# interfaces
.implements Landroid/media/MediaPlayer$OnErrorListener;


# annotations
.annotation system Ldalvik/annotation/EnclosingMethod;
    value = Lcom/racas/black/CinemaActivity;->startMovie(II)V
.end annotation

.annotation system Ldalvik/annotation/InnerClass;
    accessFlags = 0x0
    name = null
.end annotation


# instance fields
.field final synthetic this$0:Lcom/racas/black/CinemaActivity;


# direct methods
.method constructor <init>(Lcom/racas/black/CinemaActivity;)V
    .locals 0
    .annotation system Ldalvik/annotation/MethodParameters;
        accessFlags = {
            0x8010
        }
        names = {
            null
        }
    .end annotation

    .line 127
    iput-object p1, p0, Lcom/racas/black/CinemaActivity$12;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-direct {p0}, Ljava/lang/Object;-><init>()V

    return-void
.end method


# virtual methods
.method public onError(Landroid/media/MediaPlayer;II)Z
    .locals 0

    .line 128
    iget-object p1, p0, Lcom/racas/black/CinemaActivity$12;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-static {p1}, Lcom/racas/black/CinemaActivity;->access$100(Lcom/racas/black/CinemaActivity;)V

    new-instance p1, Landroid/app/AlertDialog$Builder;

    iget-object p2, p0, Lcom/racas/black/CinemaActivity$12;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-direct {p1, p2}, Landroid/app/AlertDialog$Builder;-><init>(Landroid/content/Context;)V

    const-string p2, "\u062a\u0639\u0630\u0631 \u062a\u0634\u063a\u064a\u0644 \u0627\u0644\u0645\u0634\u0647\u062f"

    invoke-virtual {p1, p2}, Landroid/app/AlertDialog$Builder;->setTitle(Ljava/lang/CharSequence;)Landroid/app/AlertDialog$Builder;

    move-result-object p1

    const-string p2, "\u0623\u0639\u062f \u0641\u062a\u062d \u0627\u0644\u0645\u0634\u0647\u062f. \u0625\u0630\u0627 \u062a\u0643\u0631\u0631 \u0627\u0644\u062e\u0637\u0623\u060c \u0623\u0639\u062f \u062a\u062b\u0628\u064a\u062a \u062a\u062d\u062f\u064a\u062b \u0627\u0644\u062a\u0637\u0628\u064a\u0642 \u0641\u0648\u0642 \u0627\u0644\u0646\u0633\u062e\u0629 \u0627\u0644\u062d\u0627\u0644\u064a\u0629 \u062f\u0648\u0646 \u062d\u0630\u0641 \u0628\u064a\u0627\u0646\u0627\u062a\u0647."

    invoke-virtual {p1, p2}, Landroid/app/AlertDialog$Builder;->setMessage(Ljava/lang/CharSequence;)Landroid/app/AlertDialog$Builder;

    move-result-object p1

    const-string p2, "\u062d\u0633\u0646\u0627\u064b"

    const/4 p3, 0x0

    invoke-virtual {p1, p2, p3}, Landroid/app/AlertDialog$Builder;->setPositiveButton(Ljava/lang/CharSequence;Landroid/content/DialogInterface$OnClickListener;)Landroid/app/AlertDialog$Builder;

    move-result-object p1

    invoke-virtual {p1}, Landroid/app/AlertDialog$Builder;->show()Landroid/app/AlertDialog;

    const/4 p1, 0x1

    return p1
.end method
